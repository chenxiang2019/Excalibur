/****************************************
           Excalibur Demo
****************************************/

#include <core.p4>
#include <v1model.p4>
typedef bit<9> Loop_PortId_t;

header ethernet_t {
  bit<48> dstAddr;
  bit<48> srcAddr;
  bit<16> etherType;
}
header ipv4_t {
  bit<4>  version;
  bit<4>  ihl;
  bit<8>  diffserv;
  bit<16> totalLen;
  bit<16> identification;
  bit<3>  flags;
  bit<13> fragOffset;
  bit<8>  ttl;
  bit<8>  protocol;
  bit<16> hdrChecksum;
  bit<32> srcAddr;
  bit<32> dstAddr;
}
struct headers {
  ethernet_t ethernet;
  ipv4_t     ipv4;
}


struct metadata_t {
  bit<1>  is_loop;
  bit<8>  color;
  bit<48> told;
  bit<48> tdif;
  bit<16> testing_status;
  bit<8>  testing_number_count;
  bit<64> testing_register_value;
}
struct metadata {
  metadata_t md;
}

parser ParserImpl(packet_in packet, out headers hdr, inout metadata meta, inout standard_metadata_t standard_metadata) {
  state parse_ethernet {
      packet.extract(hdr.ethernet);
      transition select(hdr.ethernet.etherType) {
          16w0x800: parse_ipv4;
          default: accept;
      }
  }
  state parse_ipv4 {
      packet.extract(hdr.ipv4);
      transition accept;
  }
  state start {
      transition parse_ethernet;
  }
}


control ingress(inout headers hdr, inout metadata meta, inout standard_metadata_t standard_metadata) {
  meter(32w1, MeterType.bytes) rate_limiter;
  register<bit<8>>(32w1) test_number_count_register;
  register<bit<64>>(32w1) testing_controller_register;
  action calculate_tdif() {
    testing_controller_register.read(meta.md.testing_register_value, (bit<32>)32w0);
    meta.md.testing_status = (bit<16>)((meta.md.testing_register_value & 64w0xffff000000000000) >> 48);
    meta.md.told = (bit<48>)(meta.md.testing_register_value & 64w0xffffffffffff);
    meta.md.tdif = standard_metadata.ingress_global_timestamp - meta.md.told;
  }
  action duration_control() {
    meta.md.testing_register_value = (bit<64>)standard_metadata.ingress_global_timestamp;
    testing_controller_register.write((bit<32>)32w0, (bit<64>)meta.md.testing_register_value);
  }
  //interval : 1s 
  action interval_control() {
    meta.md.testing_register_value = (bit<64>)standard_metadata.ingress_global_timestamp;
    testing_controller_register.write((bit<32>)32w0, (bit<64>)(meta.md.testing_register_value | 64w0x1000000000000));
  }
  action termination_control() {
    test_number_count_register.read(meta.md.testing_number_count, (bit<32>)32w0);
    meta.md.testing_number_count = meta.md.testing_number_count + 8w1;
    test_number_count_register.write((bit<32>)32w0, (bit<8>)meta.md.testing_number_count);
  }
  action rate_control() {
    rate_limiter.execute_meter(32w0, meta.md.color);
  }
  action do_recirulation() {
    resubmit({ meta.md.is_loop });
  }
  action drop_packet() {
    mark_to_drop(standard_metadata);
  }
  action set_loop_and_recirculate() {
    meta.md.is_loop = 1w1;
    resubmit({ meta.md.is_loop });
  }
  action packet_forward() {
    standard_metadata.egress_spec = 2;
  }
//   action ipv4_forward() {
//       standard_metadata.egress_spec = 2;         //将输出的端口从参数中取出，参数是由控制面配置
//   }



  table calculate_tdif_MAT {
   actions = {
      calculate_tdif;
    }
    const default_action = calculate_tdif();
  }
  table duration_control_MAT {
    actions = {
      duration_control;
    }
    const default_action = duration_control();
  }
  table interval_control_MAT {
    actions = {
      interval_control;
    }
    const default_action = interval_control();
  }
  table meter_MAT {
    actions = {
      rate_control;
    }
    const default_action = rate_control();
  }
  table recirculation_MAT {
    actions = {
      set_loop_and_recirculate;
    }
    const default_action = set_loop_and_recirculate();
  }
  table termination_control_MAT {
    actions = {
      termination_control;
    }
    const default_action = termination_control();
  }
  table packet_forward_MAT {
    // key = {
    //   standard_metadata.ingress_port : exact; 
    // }
    actions = {
      packet_forward;
    }
    const default_action = packet_forward();
  }
  /* Note: we define a replica of packet_forward_MAT
     to avoid the compilation error of multiple table invocation */
  table packet_forward_MAT_replica {
    // key = {
    //   standard_metadata.ingress_port : exact; 
    // }
    actions = {
      packet_forward;
    }
    const default_action = packet_forward();
  }
  table drop_packet_MAT {
    actions = {
      drop_packet;
    }
    const default_action = drop_packet();
  }
 
  apply {
    calculate_tdif_MAT.apply();
    if (meta.md.testing_status == 16w1) {
      if (meta.md.tdif >= 48w3000) {
        duration_control_MAT.apply();
        packet_forward_MAT.apply();
      }
    } else {
      if (meta.md.testing_status == 16w0) {
        if (meta.md.tdif >= 48w3000) {
          termination_control_MAT.apply();
          if (meta.md.testing_number_count >= 8w100) {
            drop_packet_MAT.apply();
          } else {
            interval_control_MAT.apply();
            packet_forward_MAT.apply();
          }
        }
      }
    }
    meter_MAT.apply();
    if (meta.md.color == 8w2) {
      packet_forward_MAT_replica.apply();
    }

    // packet_forward_MAT.apply();
  }
}


control egress(inout headers hdr, inout metadata meta, inout standard_metadata_t standard_metadata) {
  apply { }
}
control verifyChecksum(inout headers hdr, inout metadata meta) {
  apply { }
}
control computeChecksum(inout headers hdr, inout metadata meta) {
  apply { }
}
control DeparserImpl(packet_out packet, in headers hdr) {
  apply {
      packet.emit(hdr.ethernet);
      packet.emit(hdr.ipv4);
  }
}
V1Switch(ParserImpl(), verifyChecksum(), ingress(), egress(), computeChecksum(), DeparserImpl()) main;


