# -*- coding:utf8 -*-
import socket  
import sys

################
# build socket #
################
sender_addr = ('127.0.0.1', 31500)
receiver_addr = ('127.0.0.1', 8500)
agent_addr = ('127.0.0.1', 8700) 
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  
s.bind(receiver_addr)
#############
# open file #
#############
file_sort, addr = s.recvfrom(1200)
file_name = "resource" + file_sort
fp = open(file_name, "w")
#############
# arguments	#
#############
buf = []
Max = 128
cnt = 0					#cnt為期望收到的packet

##########################
# read data and send ack #
##########################
while True:
	data, addr = s.recvfrom(1200)				#recvfrom 回傳tuple
	if data == '<(__)>':
		print("recv fin")
		s.sendto('<(__)>', agent_addr)
		print("send finack")
		break

	for i in range(1,len(data)):		#讀字串和packet id
		if data[i] == '@':
			real_msg = data[i+1:]
			packet_id = data[1:i]
			break	
	if int(packet_id) == cnt:
		print("recv data #%s" %(packet_id))
		buf.append(real_msg)
		s.sendto(packet_id, agent_addr)
		print("send ack #%s" %(packet_id))
		cnt += 1
	else:
		print("drop data #%s" %(packet_id))
		s.sendto(str(cnt-1), agent_addr)
		print("send ack #%d" %(cnt-1))

	if len(buf) == Max:
		for i in range(Max):
			fp.write(buf[i])
		buf = []

if len(buf) != 0:						#若buf不為空，將剩下的寫入檔案
	for str1 in buf:
		fp.write(str1)
print("flush")
fp.close()
s.close()
