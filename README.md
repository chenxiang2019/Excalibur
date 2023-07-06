# Excalibur: A Scalable and Low-Cost Testing Framework for Evaluating DDoS Defense Solutions

To date, security researchers evaluate their solutions of mitigating distributed denial-of-service (DDoS) attacks via kernel-based or kernel-bypassing testing tools. However, kernelbased tools exhibit poor scalability in attack traffic generation while kernel-bypassing tools incur unacceptable monetary cost. We propose Excalibur, a scalable and low-cost testing framework for evaluating DDoS defense solutions. The key idea is to leverage the emerging programmable switch to empower testing tasks with Tbps-level scalability and low cost. Specifically, Excalibur offers intent-based primitives to enable academic researchers to customize testing tasks on demand. Moreover, in view of switch resource limitations, Excalibur coordinates both a server and a programmable switch to jointly perform testing tasks. It realizes flexible attack traffic generation, which requires a large number of resources, in the server while using the switch to increase the sending rate of attack traffic to Tbps-level. We have implemented Excalibur on a 64×100 Gbps Tofino switch. Our experiments on a 64×100 Gbps Tofino switch show that Excalibur achieves orders-of-magnitude higher scalability and lower cost than existing tools.

This repo contains the source code of Excalibur that targets a software-based programmable switch, BMv2.



The virtual network topology of this project includes two hosts, h1 and h2, and one switch, s1. Traffic is sent from h1, passes through s1, and reaches h2.

## 1.Environment Setup

### mininet Installation.

Get the Mininet source code in the project directory.

```
git clone http://github.com/mininet/mininet.git
```

Source code installation.

```
cd mininet/util 
./install.sh -a
```

### Install and compile p4c and bmv2.

 Installation and compilation reference for p4c: 

https://github.com/p4lang/p4c

 Installation reference for bmv2https://github.com/p4lang/behavioral-model，

For an ubuntu environment, enter the following command to install bmv2

```
. /etc/os-release
echo "deb http://download.opensuse.org/repositories/home:/p4lang/xUbuntu_${VERSION_ID}/ /" | sudo tee /etc/apt/sources.list.d/home:p4lang.list
curl -fsSL "https://download.opensuse.org/repositories/home:p4lang/xUbuntu_${VERSION_ID}/Release.key" | gpg --dearmor | sudo tee /etc/apt/trusted.gpg.d/home_p4lang.gpg > /dev/null
sudo apt update
sudo apt install p4lang-bmv2
```



## 2. A brief description of the files required to run

### bmv2 forder

Contains the P4 code required to run the bmv2 switch. Where the P4 code is written using the p4-16 standard.

### output forder

Store the compiled p4i and json files from the P4 code

### script forder

1. holds the topo.py file written with mininet, defining the topology of the virtual network environment

2. holds the source code needed to run P4_runtime. Most of these files, with the exception of topo.py, are from a copy of the official example runtime environment

A copy of https://github.com/p4lang/tutorials/tree/master/utils

### run_bmv2.sh

The project's runtime script, with the SWITCH_PATH variable replaced by the path of the simple_switch_grpc's own runtime environment

### simple_switch_CLI

simple_switch_CLI is a control script that comes with bmv2, corresponding to simple_switch, and will enter a new command line when run. 

Note that the path after sys.path.append should be changed to the path of the local python third-party library installation

### send.py

Simulating the sending of data packets

### listen.py

Receive packets and keep statistics on traffic rate, transmission interval and duration

### gen.py

Template packets were generated according to the original language read from primitives.txt. In this experiment, a trace.pcap traffic dataset containing 6000 packets was generated

## 3. Running

In the project directory, enter

```
./run_bmv2.sh
```

Start the environment and the interactive command line interface of mininet appears



Type xterm h1 h2 to open the configuration of hosts h1 and h2

In the Node : h1 terminal type

```
python3 send.py
```

Start sending the template packets in the trace, where the scapy send packet inter is set to 0.01 seconds, theoretically 100 packets are sent in 1 second



After 10s, in the Node: h2 terminal, type

```
python3 listen.py
```

Counting traffic at the receiving end

### Test flow rate



When performing traffic rate statistics, note that the apply part of the control ingress module in the p4 code is changed to the following form:

```
apply {
    packet_forward_MAT.apply();
  }
```

This allows for continuous packet forwarding



### Test interval & duration



When making interval & duration counts, note that the apply part of the control ingress module in the p4 code is changed to the following form:

```
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
```

This sets the traffic forwarding interval to 1 second and the duration to 3 seconds



The test interval & duration script outputs a list of 50 elements containing the values 0 and 1, indicating whether packets were received in each 1 second slice of time for the first 50 seconds of packet reception at the receiving end.



## 4.Results

The results are saved in the pic folder.

where 1.png is the screenshot after running the run_bmv2.sh script

2.png is the result of the traffic rate test, you can see that the reception rate of h2 is close to the theoretical sending rate of h1

3.png is the result of the interval & duration test, you can see that there are generally 3 consecutive 1's and 1 0's, which matches the code that the switch traffic is forwarded for 3 seconds with a 1 second pause.