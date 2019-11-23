# -*- coding:utf8 -*-
import socket
import sys
import os

################
# build socket #
################
sender_addr = ('127.0.0.1', 31500)
receiver_addr = ('127.0.0.1', 8500)
agent_addr = ('127.0.0.1', 8700) 
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  
s.bind(agent_addr)
#############
# arguments	#
#############
def_loss_rate = 0.01
get_num = 0.0		#從sender拿到的packet總數（不管有無loss）
loss_num = 0.0
cur_loss_rate = 0.0					
sender_data = ''
receiver_data = ''
finish = 0
real_get = 0	#不包括loss的packet數目
##########################
# get data and fwd ack   #
##########################
all_pkt_num, addr1 = s.recvfrom(1200)
while not finish:
	real_trans = 0								#此輪實際傳送給receiver的data數目
	window_size, addr1 = s.recvfrom(1200)
	#print("window_size = %s" %(window_size))
	if window_size == '<(__)>':
		print("get fin")
		s.sendto('<(__)>', receiver_addr)
		real_trans += 1
		print("fwd fin")
	else:
		has_loss = 0 							#紀錄這輪有沒掉包
		sender_msg = []							#存放此輪收到的packet
		this_pkt_num = int(window_size)					#判斷windows是否比剩下的封包還大
		if int(window_size) > int(all_pkt_num) - real_get:
			this_pkt_num = int(all_pkt_num) - real_get

		flag = 0
		while flag < this_pkt_num:
			sender_data, addr1 = s.recvfrom(1200)
			sender_msg.append(sender_data)
			flag += 1

		for i in range(len(sender_msg)):
			get_num += 1.0
			cur_loss_rate = float(loss_num/get_num)
			
			for j in range(1,len(sender_msg[i])):
				if sender_msg[i][j] == '@':
					packet_id = sender_msg[i][1:j]
					real_msg = sender_msg[i][j+1:]
					break
			
			print("get data #%s" %(packet_id))

			if cur_loss_rate < def_loss_rate:
				print("drop data #%s loss rate = %f" %(packet_id, cur_loss_rate))
				loss_num += 1.0
				has_loss = 1
				continue
			else:
				s.sendto(sender_msg[i],receiver_addr)
				print("fwd data #%s loss rate = %f" %(packet_id, cur_loss_rate))
				real_trans += 1
				if not has_loss:
					real_get += 1

	get_ack_num = 0
	while get_ack_num < real_trans:
		receiver_data, addr2 = s.recvfrom(1200)
		if receiver_data == '<(__)>':
			print("get finack")
			s.sendto('<(__)>', sender_addr)
			print("fwd finack")
			get_ack_num += 1
			finish = 1
	
		else:
			get_ack_num += 1
			print("get ack #%s" %(receiver_data))
			s.sendto(receiver_data, sender_addr)
			print("fwd ack #%s" %(receiver_data))

s.close()
