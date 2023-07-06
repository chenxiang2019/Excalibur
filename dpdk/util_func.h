#include "util_text.h"


//Ethernet,IPv4,TCP,UDP,Payload



//static void (*(func[10]))(struct rte_mbuf *);
//static int func_len = 0;  //modify func
struct my_header {
	uint16_t id;
	uint16_t flag;
};
static void
fill_my_header(struct my_header *hdr) {
	hdr->id = rte_cpu_to_be_16(1);
	hdr->flag = rte_cpu_to_be_16(1234);
}

static void
fill_ethernet_header(struct rte_ether_hdr *hdr) {
	struct rte_ether_addr s_addr = {{0x14,0x02,0xEC,0x89,0x8D,0x24}};
	struct rte_ether_addr d_addr = {{0x00,0x50,0x56,0x97,0x5A,0xBF}};
	hdr->s_addr =s_addr;
	hdr->d_addr =d_addr;
	hdr->ether_type = rte_cpu_to_be_16(0x0800);
}

static void
fill_ipv4_header(struct rte_ipv4_hdr *hdr) {
	hdr->version_ihl = (4 << 4) + 5; // ipv4, length 5 (*4)
	hdr->type_of_service = 0; // No Diffserv
	hdr->total_length = rte_cpu_to_be_16(40); // tcp 20
	hdr->packet_id = rte_cpu_to_be_16(5462); // set random
	hdr->fragment_offset = rte_cpu_to_be_16(0);
	hdr->time_to_live = 64;
	hdr->next_proto_id = 6; // tcp
	hdr->hdr_checksum = rte_cpu_to_be_16(25295);
	hdr->src_addr = rte_cpu_to_be_32(0xC0A80001); // 192.168.0.1
	hdr->dst_addr = rte_cpu_to_be_32(0x01010101); // 1.1.1.1
}

static void
fill_tcp_header(struct rte_tcp_hdr *hdr) {
	hdr->src_port = rte_cpu_to_be_16(0x162E);
	hdr->dst_port = rte_cpu_to_be_16(0x04d2);
	hdr->sent_seq = rte_cpu_to_be_32(0);
	hdr->recv_ack = rte_cpu_to_be_32(0);
	hdr->data_off = 0;
	hdr->tcp_flags = 0;
	hdr->rx_win = rte_cpu_to_be_16(16);
	hdr->cksum = rte_cpu_to_be_16(0);
	hdr->tcp_urp = rte_cpu_to_be_16(0);
}

static void Ethernet_update(struct rte_mbuf *m ,struct rte_ether_hdr *hdr){
	//printf("Ethernet_update call successfully\n");
	
	hdr = (struct rte_ether_hdr *) rte_pktmbuf_append(m, sizeof(struct rte_ether_hdr));
	
	struct rte_ether_addr s_addr = {{0x14,0x02,0xEC,0x89,0x8D,0x24}};
	hdr->s_addr =s_addr;

	struct rte_ether_addr d_addr = {{0x14,0x02,0xEC,0x89,0x8D,0x24}};
	hdr->d_addr =d_addr;

	hdr->ether_type = rte_cpu_to_be_16(0x0800);
}

static void IPV4_update(struct rte_mbuf *m){
	//printf("IPV4_update call successfully\n");
	struct rte_ipv4_hdr *hdr;
	hdr = (struct rte_ipv4_hdr *) rte_pktmbuf_append(m, sizeof(struct rte_ipv4_hdr));
	hdr->version_ihl = (4 << 4) + 5; // ipv4, length 5 (*4)
	hdr->type_of_service = 0; // No Diffserv
	hdr->total_length = rte_cpu_to_be_16(40); // tcp 20
	hdr->packet_id = rte_cpu_to_be_16(5462); // set random
	hdr->fragment_offset = rte_cpu_to_be_16(0);
	hdr->time_to_live = 64;
	hdr->next_proto_id = 6; // tcp
	hdr->hdr_checksum = rte_cpu_to_be_16(25295);
	hdr->src_addr = rte_cpu_to_be_32(0xC0A80001); // 192.168.0.1
	hdr->dst_addr = rte_cpu_to_be_32(0x01010101); // 1.1.1.1
	
}
static void TCP_update(struct rte_mbuf *m){
	//printf("TCP_update call successfully\n");
	struct rte_tcp_hdr *hdr;
	hdr = (struct rte_tcp_hdr *) rte_pktmbuf_append(m, sizeof(struct rte_tcp_hdr));
	hdr->src_port = rte_cpu_to_be_16(0x162E);
	hdr->dst_port = rte_cpu_to_be_16(0x04d2);
	hdr->sent_seq = rte_cpu_to_be_32(0);
	hdr->recv_ack = rte_cpu_to_be_32(0);
	hdr->data_off = 0;
	hdr->tcp_flags = 0;
	hdr->rx_win = rte_cpu_to_be_16(16);
	hdr->cksum = rte_cpu_to_be_16(0);
	hdr->tcp_urp = rte_cpu_to_be_16(0);
}

static void UDP_update(struct rte_mbuf *m){
	//printf("UDP_update call successfully\n");
	struct rte_udp_hdr *hdr;
	hdr = (struct rte_udp_hdr *) rte_pktmbuf_append(m, sizeof(struct rte_udp_hdr));
	hdr->src_port = rte_cpu_to_be_16(0x162E);
	hdr->dst_port = rte_cpu_to_be_16(0x162E);
	hdr->dgram_len = 0;
}

/* 
void  Ethernet_srcAddr_update () {
	struct rte_ether_addr s_addr = {{0x14,0x02,0xEC,0x89,0x8D,0x24}};
	ether_hdr->s_addr =s_addr;
}
void Ethernet_dstAddr_update () {
	struct rte_ether_addr d_addr = {{0x14,0x02,0xEC,0x89,0x8D,0x24}};
	ether_hdr->d_addr =d_addr;
}
void Ethernet_ethertype_update () {
	ether_hdr->ether_type = rte_cpu_to_be_16(0x0800);
}
void IPv4_srcIP_update () {
	ipv4_hdr->src_addr = rte_cpu_to_be_32(0xC0A80001);
}
void IPv4_dstIP_update () {
	ipv4_hdr->dst_addr = rte_cpu_to_be_32(0xC0A80001);
}
void IPv4_next_proto_update () {
	ipv4_hdr->next_proto_id = 6;
}
void IPv4_total_length_update () {
	ipv4_hdr->total_length = rte_cpu_to_be_16(40);
}
void TCP_srcPort_update () {
	tcp_hdr->src_port = rte_cpu_to_be_16(0x162E);
}
void TCP_dstPort_update () {
	tcp_hdr->dst_port = rte_cpu_to_be_16(0x04d2);
}
void TCP_flag_update () {
	tcp_hdr->tcp_flags = 0;
}
void UDP_srcPort_update () {
	udp_hdr->src_port = rte_cpu_to_be_16(0x162E);
}
void UDP_dstPort_update () {
	udp_hdr->dst_port = rte_cpu_to_be_16(0x162E);
}
void UDP_dgram_len_update () {
	udp_hdr->dgram_len = 0;
}
*/

/**
void Func_arrange(){
	int header_len = 5;
	int i,j;
	 for (i=0;i<header_len;i++){
	 	if( strcmp(head[i].field,"Ethernet")==0 ){
			func[func_len] = Ethernet_update ;
			func_len++;
			/*for(j=0;j<head[i].length;j++){
				if( strcmp(head[i].field[j],"Ethernet.srcaddr")==0 ){
					func[func_len] =  Ethernet_srcAddr_update; 
					func_len++;
				}
					// 添加将调用的函数
				else if ( strcmp(head[i].field[j],"Ethernet.dstaddr")==0 ){
					func[func_len] =  Ethernet_dstAddr_update; 
					func_len++;
				}
					// 添加将调用的函数
				else if ( strcmp(head[i].field[j],"Ethernet.ether_type")==0 ){
					func[func_len] =  Ethernet_ethertype_update; 
					func_len++;
				}
					// 添加将调用的函数 	
			  }*/		
	//	}else if( strcmp(head[i].field,"IPv4")==0 ){
		//	func[func_len] = IPV4_update ;
			//func_len++;
			/*for(j=0;j<head[i].length;j++){
				if( strcmp(head[i].field[j],"IPv4.srcIP")==0 ){
					func[func_len] =  IPv4_srcIP_update; 
					func_len++;
				}
					// 添加将调用的函数 
				else if ( strcmp(head[i].field[j],"IPv4.dstIP")==0 ){
					func[func_len] =  IPv4_dstIP_update; 
					func_len++;
				}
					// 添加将调用的函数
				else if ( strcmp(head[i].field[j],"IPv4.next_type")==0 ) {
					func[func_len] =  IPv4_next_proto_update; 
					func_len++;
				}
					//添加将调用的函数
				else if ( strcmp(head[i].field[j],"IPv4.ipv4_len")==0 ){
					func[func_len] =  IPv4_total_length_update; 
					func_len++;
				}
					//添加将调用的函数
			}*/
			// checksum 计算   
			//func_len++;
	//	}else if( strcmp(head[i].field,"TCP")==0 ){
	//		func[func_len] = TCP_update ;
	//		func_len++;
			/*for(j=0;j<head[i].length;j++){
				if( strcmp(head[i].field[j],"TCP.flag")==0 ){
					func[func_len] =  TCP_flag_update; 
					func_len++;
				}
					// 添加将调用的函数 
				else if ( strcmp(head[i].field[j],"TCP.srcPort")==0 ){
					func[func_len] =  TCP_srcPort_update; 
					func_len++;
				}
					// 添加将调用的函数
				else if ( strcmp(head[i].field[j],"TCP.dstPort")==0 ){
					func[func_len] =  TCP_dstPort_update; 
					func_len++;
				}
					// 添加将调用的函数
			} */
//		}else if( strcmp(head[i].field,"UDP")==0 ){
//			func[func_len] = UDP_update ;
//			func_len++;
			/*for(j=0;j<head[i].length;j++){
				if( strcmp(head[i].field[j],"UDP.srcPort")==0 ){
					func[func_len] =  UDP_srcPort_update; 
					func_len++;
				}
					// 添加将调用的函数 
				else if ( strcmp(head[i].field[j],"UDP.dstPort")==0 ){
					func[func_len] =  UDP_dstPort_update; 
					func_len++;
				}
					// 添加将调用的函数
				else if ( strcmp(head[i].field[j],"UDP.udp_len")==0 ){
					func[func_len] =  UDP_dgram_len_update; 
					func_len++;
				}
					// 添加将调用的函数
			}*/
			// checksum 计算
			//func_len ++; 
//		}else if( strcmp(head[i].field,"Payload")==0 ){
					// 添加将调用的函数 忘记怎么添加了   
//		}else {
//			printf("error , unknown input header\n");
//		}
//	 }
//}
//

/*
static struct rte_ether_hdr *ether_hdr;
static struct rte_ipv4_hdr *ipv4_hdr;
static struct rte_tcp_hdr *tcp_hdr;
static struct rte_udp_hdr *udp_hdr;
for (i = 0; i < burst_size; i++){
		//申请空间 索引赋值给各个静态头部指针  执行初始化
		m = rte_pktmbuf_alloc(l2fwd_pktmbuf_pool);
		
		ether_hdr = (struct rte_ether_hdr *) rte_pktmbuf_append(m, sizeof(struct rte_ether_hdr));
		init_ethernet_header(ether_h);  // 

		ipv4_hdr = (struct rte_ipv4_hdr *) rte_pktmbuf_append(m, sizeof(struct rte_ipv4_hdr));
		init_ipv4_header(ipv4_h);  //

		tcp_hdr = (struct rte_tcp_hdr *) rte_pktmbuf_append(m, sizeof(struct rte_tcp_hdr));
		init_tcp_header(tcp_h); //
		
		udp_hdr = (struct rte_udp_hdr *) rte_pktmbuf_append(m, sizeof(struct rte_udp_hdr));
		init_tcp_header(udp_h);  //
		
		// 按函数调用数组顺序执行函数 具体修改各个字段
		for ( j=0 ; j<func_len ; j++ ) 
			(*(func[j]))();
		
		rte_eth_tx_buffer(dst_port, 0, buffer, m);
	}
*/


