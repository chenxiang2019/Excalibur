import os
from scapy.all import *

#pkt = Ether(dst="00:04:00:00:00:02")/IP(dst="10.0.0.2")/UDP(dport=1234)
# pkt = Ether(dst="00:04:00:00:00:02")/IP(src = '10.' + str(random.randrange(0, 255, 1)) + '.' + str(random.randrange(0, 255, 1)) + '.' + str(random.randrange(0, 255, 1)),dst="10.0.0.2")/TCP(dport=1234,flags='S')/'1111111111111111111111111'



pkt = rdpcap('trace.pcap')
# pkt_len = len(pkt[0])
print(len(pkt))
sendp(pkt,inter = 0.01,iface='eth0')






# pkt= Ether(dst='00:04:00:00:00:02')/IP(dst='10.0.0.2')/UDP()
# sendpfast(pkt, pps=100,loop=1000, file_cache = True)
# {
#  'command': 'tcpreplay --timer=nano --pps=100 -intf1=eth0 --loop=0 /tmp/scapyo6ilzjdk',
#  'warnings': []
#  }
