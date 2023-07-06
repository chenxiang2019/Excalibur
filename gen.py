import os
from scapy.all import *
from scapy.layers.inet import TCP, IP, UDP
from scapy.layers.l2 import Ether
from scapy.utils import PcapWriter
import random
import ipaddress
import socket

port = 0
rate = 0
Ntest = 0
duration = 0
interval = 0
total_num = 6000


dst_mac = "00:04:00:00:00:02"
def set_packet_structure(para):
    # global pkt
    para[0] = para[0][2:]
    para[-1] = para[-1][:-2]
    headers = para
    # print(headers+'1111111111111')
    # print(headers)
    return headers


def select_field(para):
    para[0] = para[0][2:]
    para[-1] = para[-1][:-2]
    field = para
    # print(para)
    return field


def set_field_value(para):
    new_list = []
    new_para = []
    flag = True

    start = 0
    end = 0
    Indexes = []
    for index, i in enumerate(para):
        if i[0] == '[':
            start = index
            i = i[1:]
        if i[-1] == ']':
            end = index
            Indexes.append([start, end])
            i = i[:-1]
            end = 0
            start = 0

    for indexes in Indexes:
        para[indexes[0]] = para[indexes[0]][1:]
        para[indexes[1]] = para[indexes[1]][:-1]
        # print(para[indexes[0]])
        # print(para[indexes[1]])
        #
        # print(indexes[0],indexes[1]+1)
        # print('------------')
        # for j in range(indexes[0]+1,indexes[1]):
        #     para[j] = para[j][1:-1]
        new_para.append(para[indexes[0]:indexes[1] + 1])
    # for word in para:
    #     print(word)
    # for word in new_para:
    #     print(word)
    # print(new_para)

    return new_para[0], new_para[1]


def set_packet_length(para):
    length = int(para[0])
    return length


def set_Prob(para):
    k, field, min, max = int(para[0]), para[1], float(para[2]), float(para[3])
    return k, field, min, max


def replay_Trace(headers, selectedField, field, value, length, k, prob_field, u_min, u_max):
    # 数据包的数量


    # set_prob 怎么写？
    lambda_num = int(0.5 * (u_max + u_min) * total_num)
    per_topk_flow_packets_num = int(lambda_num / k) - 1
    random_attack_ip = ''
    pkt = 0
    # 生成正常包
    for i in range(total_num - lambda_num):
        if headers[0] == 'Ethernet':
            pkt = Ether()
        if headers[1] == 'IPv4':
            pkt = Ether() / IP()
        if headers[2] == 'TCP':
            pkt = Ether() / IP() / TCP()
        elif headers[2] == 'UDP':
            pkt = Ether() / IP() / UDP()

        if selectedField[0] == 'IPv4.srcIP':
            a = random.randrange(0, 255, 1)
            b = random.randrange(0, 255, 1)
            c = random.randrange(0, 255, 1)
            random_attack_ip = '10.' + str(a) + '.' + str(b) + '.' + str(c)
            # print(random_attack_ip)
            pkt = Ether() / IP(src=random_attack_ip) / TCP()

        if field[0] == 'IPv4.dstIP':
            a = random.randrange(0, 255, 1)
            b = random.randrange(0, 255, 1)
            c = random.randrange(0, 255, 1)
            random_attack_ip = '10.' + str(a) + '.' + str(b) + '.' + str(c)

            pkt = Ether(dst=dst_mac) / IP(src=random_attack_ip, dst=value[0]) / TCP()

        if field[1] == 'TCP.flags':
            a = random.randrange(0, 2, 1)
            b = random.randrange(2, 4, 1)
            c = random.randrange(4, 6, 1)
            random_attack_ip = '10.' + str(a) + '.' + str(b) + '.' + str(c)
            print(value[1])
            pkt = Ether(dst=dst_mac) / IP(src=random_attack_ip, dst=value[0]) / TCP(flags=value[1])

        # 修改数据包长度

        if len(pkt) < length:
            payload_length = length - len(pkt)
            payload_content = '\x00' * payload_length
            pkt = pkt / payload_content

     
        wrpcap('trace.pcap', pkt, append=True)

    # 生成top-k flow包
    if prob_field == 'IPv4.srcIP':
        for i in range(k):
            a = random.randrange(0, 2, 1)
            b = random.randrange(2, 4, 1)
            c = random.randrange(4, 6, 1)
            for j in range(per_topk_flow_packets_num):
                random_attack_ip = '10.' + str(a) + '.' + str(b) + '.' + str(c)
                # random_attack_ip = socket.inet_aton(random_attack_ip)
                pkt = Ether(dst=dst_mac) / IP(src=random_attack_ip, dst=value[0]) / TCP(flags=value[1])

                if len(pkt) < length:
                    payload_length = length - len(pkt)
                    payload_content = '\x00' * payload_length
                    random_attack_ip = '10.' + str(a) + '.' + str(b) + '.' + str(c)
                    # random_attack_ip = socket.inet_aton(random_attack_ip)

                    pkt = Ether(dst=dst_mac) / IP(src=random_attack_ip, dst=value[0]) / TCP(flags=value[1]) / payload_content
                # 数据包超出长度截断
                wrpcap('trace.pcap', pkt, append=True)

    return 'trace.pcap'


def set_port(para):
    global port
    port = int(para[0])
    return


def set_Rate(para):
    global rate
    rate = int(para[0])
    return


def Set_Number(para):
    global Ntest
    Ntest = int(para[0])
    return


def Set_Duration(para):
    global duration
    duration = int(para[0])
    return


def Set_Interval(para):
    global interval
    interval = int(para[0])
    return


def parse_line(line):
    primitives_name = line.split('(')[0]
    parameter_list = line.split('(')[1].split(')')[0]
    if parameter_list.find(',') != -1:
        tmp_list = parameter_list.split(',')
        new_list = []
        for k in tmp_list:
            new_k = k.lstrip()
            new_list.append(new_k)
        primitives_parameter_list = new_list
    else:
        primitives_parameter_list = [parameter_list]
    return primitives_name, primitives_parameter_list


def parser(filename):
    with open(filename) as f:
        lines = f.readlines()
    # print(lines)

    primitives_name_list = []
    headers, selectedField, field, value, length, k, prob_field, u_min, u_max = 0, 0, 0, 0, 0, 0, 0, 0, 0
    for i in lines:
        primitives_name, primitives_parameter_list = parse_line(i)
        # print(primitives_name)
        # print(primitives_parameter_list)
        # print('---------------')
        if primitives_name == 'Set Packet Structure':
            headers = set_packet_structure(primitives_parameter_list)
        elif primitives_name == 'Select Field':
            selectedField = select_field(primitives_parameter_list)
        elif primitives_name == 'Set Field Value':
            field, value = set_field_value(primitives_parameter_list)
        elif primitives_name == 'Set Packet Length':
            length = set_packet_length(primitives_parameter_list)

        elif primitives_name == 'Set Prob':
            k, prob_field, u_min, u_max = set_Prob(primitives_parameter_list)

        elif primitives_name == 'Replay Trace':
            filename = replay_Trace(headers, selectedField, field, value, length, k, prob_field, u_min, u_max)

        # 此时已完成数据包的生成

        if primitives_name == 'Set Port':
            set_port(primitives_parameter_list)
        elif primitives_name == 'Set Rate':
            set_Rate(primitives_parameter_list)
        elif primitives_name == 'Set Number':
            Set_Number(primitives_parameter_list)
        elif primitives_name == 'Set Duration':
            Set_Duration(primitives_parameter_list)
        elif primitives_name == 'Set Interval':
            Set_Interval(primitives_parameter_list)

    # print(primitives_name_list)
    # ['Set Packet Structure', 'Select Field', 'Set Field Value', 'Set Packet Length', 'Set Prob',
    # 'Set Port', 'Set Rate', 'Set Number', 'Set Duration', 'Set Interval']
    # primitives_parameter_list = []
    # for i in lines:
    #     primitives_parameter_list.append(i.split('(')[1].split(')')[0])
    # print(primitives_parameter_list)


parser('primitives.txt')