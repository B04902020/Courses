#coding: utf-8
import socket
import sys
import os
fp = open('./config','r')
tmp = fp.read()
channel = tmp.split("'")[1]
server = "irc.freenode.net"
nickname = "robot666"
ans = []
class IRC:
 
    irc = socket.socket()
  
    def __init__(self):  
        self.irc = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
 
    def send(self, chan, msg):
        send_str = bytes("PRIVMSG " + chan + " :" + msg + "\r\n","utf8")
        self.irc.send(send_str)
 
    def connect(self, server, channel, botnick):
        #defines the socket
        print ("connecting to:" + server)
        self.irc.connect((bytes(server,"utf8"), 6667))                 #connects to the server
        send_str = "USER " + botnick + " " + botnick + " " + botnick + " :This is a fun bot!\r\n"
        self.irc.send(bytes(send_str,"utf8"))
        self.irc.send(bytes("nick " + botnick + "\r\n","utf8"))
                   
        self.irc.send(bytes("join " + channel + "\r\n","utf8"))       #join the channel
        
    def get_text(self):
        text = self.irc.recv(4096)  #receive the text 
        if text.find(bytes('PING',"utf8")) != -1:                      
            self.irc.send(bytes('PONG ',"utf8") + text.split()[1] + bytes('\r\n',"utf8"))
        return text
 
def check(result_ip):
    cnt = 0
    last_pos = 0
    for i in range(len(result_ip)):
        if result_ip[i] == '.' :
            num1 = int(result_ip[last_pos:i])
            if result_ip[last_pos] == '0' and result_ip[last_pos+1] != '.' :
                return 0
            elif num1 > 256 or num1 < 0:
                    return 0
            cnt += 1
            last_pos = i+1
        if cnt == 3:
            num2 = int(result_ip[i+1:])
            cnt += 1
            if result_ip[last_pos] == '0' and last_pos != len(result_ip)-1 :
                return 0
            elif num2 > 256 or num2 < 0 :
                return 0
    return 1

def dfs(cur_ip, last, dot_num):             #In the begining, last_pos = -1
    if dot_num == 3 and check(cur_ip) == 1 :
        ans.append(cur_ip)
        return
    if last+2 > len(cur_ip)-1 :
        return
    for i in range(last+2,len(cur_ip)):
        temp_ip = cur_ip[:i] + "." + cur_ip[i:]
        dfs(temp_ip, i, dot_num+1)
 
robot = IRC()
robot.connect(server, channel, nickname)
#print("connect success")
robot.send(channel, "Hello! I am a robot.")
while 1:
    text1 = robot.get_text()
    #print("I get: " + str(text1))
    if bytes("PRIVMSG","utf8") in text1 and bytes(channel,"utf8") in text1:
        
        if bytes("@repeat","utf8") in text1:
            data1 = text1[text1.index(bytes("@repeat","utf8"))+8:]
            send_text = str(data1)            #has 'b '
            send_text2 = send_text[2:len(send_text)-5]              #delete 'b \r\n' 
            robot.send(channel,send_text2)
        
        elif bytes("@convert","utf8") in text1:
            data2 = text1[text1.index(bytes("@convert","utf8"))+9:].strip()
            if data2[:2] == bytes("0x","utf8"):
                str1 = str(data2)
                str2 = str1[2:len(str1)-1]
                send_text = int(str2,16)
                robot.send(channel, str(send_text))
            else:
                temp2 = int(data2)      #bytes--->int
                send_text = hex(temp2)      #int--->16進位的字串
                robot.send(channel, send_text)
        
        elif bytes("@help","utf8") in text1:
            robot.send(channel, "@repeat <Message>")
            robot.send(channel, "@convert <Number>")
            robot.send(channel, "@ip <String>")
        
        elif bytes("@ip", "utf8") in text1:
            data3 = text1[text1.index(bytes("@ip", "utf8"))+4:].strip()
            str1 = str(data3)
            str2 = str1[2:len(str1)-1]
            dfs(str2, -1, 0)
            robot.send(channel, str(len(ans)))
            for i in ans:
               robot.send(channel, i)
            ans.clear()
