from mininet.net import Mininet
from mininet.node import Controller, OVSSwitch, RemoteController
from mininet.topo import Topo
from mininet.log import setLogLevel, info
from mininet.cli import CLI

from p4_mininet import P4Switch, P4Host
from p4runtime_switch import P4RuntimeSwitch

import argparse
from time import sleep

parser = argparse.ArgumentParser(description='Mininet demo')
parser.add_argument('--behavioral-exe', help='Path to behavioral executable',
                    type=str, action="store", required=True)
parser.add_argument('--thrift-port', help='Thrift server port for table updates',
                    type=int, action="store", default=9090)
parser.add_argument('--num-hosts', help='Number of hosts to connect to switch',
                    type=int, action="store", default=2)
parser.add_argument('--mode', choices=['l2', 'l3'], type=str, default='l3')
parser.add_argument('--json', help='Path to JSON config file',
                    type=str, action="store", required=True)
parser.add_argument('--pcap-dump', help='Dump packets on interfaces to pcap files',
                    type=str, action="store", required=False, default=False)
parser.add_argument('--cli', help='simple_switch_cli',
                    type=str, action="store", required=False, default=False)

args = parser.parse_args()

# 创建Mininet网络拓扑
net = Mininet(host = P4Host,switch = P4RuntimeSwitch,controller = None)

# 添加控制器



# 添加两个主机
h1 = net.addHost("h1", ip="10.0.0.1",mac = '00:04:00:00:00:01',defaultRoute=None,netmask='10.255.255.255')
h2 = net.addHost("h2", ip="10.0.0.2",mac = '00:04:00:00:00:02',defaultRoute=None,netmask='10.255.255.255')
s1 = net.addSwitch('s1',sw_path = args.behavioral_exe,json_path = args.json,thrift_port = args.thrift_port,pcap_dump = args.pcap_dump)
# 添加一个交换机

# 配置连接
net.addLink(h1, s1,1,1)
net.addLink(h2, s1,1,2)

# 启动网络

# s1.start()

# 配置IP地址
# h1.cmd("ifconfig h1-eth0 10.0.0.1/24")
# h2.cmd("ifconfig h2-eth0 10.0.0.2/24")
# s1.cmd("ifconfig s1-eth0 0")
# s1.cmd("ifconfig s1-eth1 0")
# h1.cmd("ip addr add 10.0.0.1/24 dev h1-eth0")
# h2.cmd("ip addr add 10.0.0.2/24 dev h2-eth0")
# s1.cmd("ip addr add 10.0.0.3/24 dev s1-eth1")
# s1.cmd("ip addr add 10.0.0.4/24 dev s1-eth2")

# h1.cmd("route add -net 10.0.0.2/24 dev h1-eth1")
# s1.cmd("route add -net 10.0.0.2/24 dev s1-eth1")
# s1.cmd("route add -net 10.0.0.2/24 dev s1-eth2")


# s1.cmd("ip addr add 10.0.0.2/24 dev s1-eth1")
# s1.cmd("ip addr add 10.0.0.2/24 dev s1-eth2")



# s1.cmd("route add -net 10.0.0.3/24 dev s1-eth1")
# s1.cmd("route add -net 10.0.0.2/24 dev s1-eth1")

# 启动交换机
# s1.cmd("sudo set bridge s1 protocols=OpenFlow13")
# s1.cmd("sudo add-flow s1 priority=1,actions=normal")
# s1.cmd("sudo add-flow s1 in_port=1,actions=output:2")
# s1.cmd("sudo add-flow s1 in_port=2,actions=output:1")

# 测试连接
# h1.cmd("ping 10.0.0.2 -c 4")
num_hosts = 2
# sw_mac = ["00:aa:bb:00:00:%02x" % n for n in range(num_hosts)]
# sw_addr = ["10.0.%d.1" % n for n in range(num_hosts)]
# for n in range(num_hosts):
#     h = net.get('h%d' % (n + 1))
#     h.setARP(sw_addr[n], sw_mac[n])
#     h.setDefaultRoute("dev eth0 via %s" % sw_addr[n])

# h1.cmd("route add default gw 10.0.0.3 dev eth0")
# h1.cmd("arp -i eth0 -s 10.0.0.3 08:00:00:00:03:00")
# h2.cmd("route add default gw 10.0.0.4 dev eth0")
# h2.cmd("arp -i eth0 -s 10.0.0.4 08:00:00:00:04:00")




for n in range(num_hosts):
    h = net.get('h%d' % (n + 1))
    h.describe()

net.start()
net.build()
# 启动命令行界面
CLI(net)

# 结束网络
net.stop()