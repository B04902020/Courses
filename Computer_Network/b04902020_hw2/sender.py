# -*- coding:utf8 -*-
import socket
import sys
import fcntl
import os
import time

##################################
# build socket and set address   #
##################################
sender_addr = ('127.0.0.1', 31500)
receiver_addr = ('127.0.0.1', 8500)
agent_addr = ('127.0.0.1', 8700)
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind(sender_addr)
fcntl.fcntl(s, fcntl.F_SETFL, os.O_NONBLOCK)
###################################
# open file and setting file_name #
###################################
main_file = sys.argv[1]
fp = open(main_file, "r")
sub_file = sys.argv[2]
s.sendto(sub_file, receiver_addr)
################
# set argument #
################
threshould = 16
window_size = 1
packet_size = 1024
cnt = 0				#正要處理的封包
packet_buf = []
check_ack = []
#########################
#  read file and split  #
#########################
while True:
	tmp = fp.read(packet_size)
	if not tmp:
		break
	packet_buf.append(tmp)

for i in range(len(packet_buf)):			#每一段的尾巴加上編號（方便receiver知道是幾號）
	packet_buf[i] = '@' + str(i) + '@' + packet_buf[i]
for i in range(len(packet_buf)):
	check_ack.append(0)
s.sendto(str(len(packet_buf)), agent_addr)
#################
# communication #
#################
while cnt < len(packet_buf):
	num_to_agent = 0
	s.sendto(str(window_size), agent_addr)
	for i in range(window_size):
		if(cnt+i < len(packet_buf)):
			s.sendto(packet_buf[cnt+i], agent_addr)
			print("send data #%d, window_size = %d" %(cnt+i,window_size))
			num_to_agent += 1

	while True: 
		t0 = time.time()
		data = ''
		get_ack_num = 0
		while time.time() - t0 <= 1 and get_ack_num < num_to_agent:	#receive ack (id of packet) set timeout
			try:
				data, addr = s.recvfrom(1024)	
				print("recv ack #%s" %(data))
				get_ack_num += 1
				if int(data) >= cnt and int(data) <= cnt+num_to_agent-1:
					check_ack[int(data)] += 1
			
			except socket.error:
				pass
   		
		expect_cnt = cnt + num_to_agent
		origin_cnt = cnt

		for i in range(num_to_agent):
			if check_ack[origin_cnt+i] == 1:
				cnt = origin_cnt + i + 1
			
			elif check_ack[origin_cnt+i] > 1:
				check_ack[origin_cnt+i] = 1
				cnt = origin_cnt + i + 1

		if cnt != expect_cnt:
			threshould = max(window_size/2, 1)
			window_size = 1
			print("time out, threshould = %d" %(threshould))
			s.sendto(str(window_size), agent_addr)
			s.sendto(packet_buf[cnt], agent_addr)
			print("resnd data #%s" %(cnt))
			num_to_agent = 1
		else:
			if window_size >= threshould:
				window_size += 1
			else:
				window_size *= 2
			break

s.sendto('<(__)>', agent_addr)
print("send fin")
t1 = time.time()
finish = 0
while finish == 0:
	while time.time() - t1 <= 1:
		try:
			data, addr = s.recvfrom(1024)
			print("recv finack")
			finish = 1
			break
		except socket.error:
			pass

fp.close()
s.close()
