import serial
import sys
import socket
from scapy.all import *

device = serial.Serial('COM6', 115200)
#s = socket.socket(socket.AF_INET, socket.SOCK_RAW)

interface = IFACES.dev_from_index(5)
socket = conf.L2socket(iface = interface)
print(interface)

#s.bind((interface,1234))


while True:
    len_bit_1 = device.read()
    len_bit_2 = device.read()
    len_msg = int.from_bytes(len_bit_1,'big') + int.from_bytes(len_bit_2,'big')
    #print(len_msg)
    #print(int.from_bytes(len_bit_1,'big') + int.from_bytes(len_bit_2,'big'))
    #data = device.read(int.from_bytes(len_bit_1,'big') + int.from_bytes(len_bit_2,'big') << 8)
    data = device.read(len_msg)
    #print(data)
    socket.send(data)

