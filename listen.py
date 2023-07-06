import os
from scapy.all import *

# speed test
timeout_seconds = 5
packet_size = 120
# pkts = sniff(timeout=timeout_seconds,iface='eth0',filter='udp')
pkts = sniff(timeout=timeout_seconds,iface='eth0',filter='tcp')
print(len(pkts))  #print the nums of packets,flow speed
flow_speed = len(pkts)*packet_size/timeout_seconds
send_speed = packet_size*100
print('send speed:'+str(send_speed)+'\n')
print(flow_speed)



#duration and interval test
time_gran = 1
time_priods = 50
is_on = [1]*time_priods
for i in range(time_priods):
	pkts = sniff(timeout=time_gran*0.9,iface='eth0',filter='tcp')
	if len(pkts) < 18:
		is_on[i] = 0

print(is_on)


