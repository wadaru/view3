#!/usr/bin/python3
import struct
import time
from socket import socket, AF_INET, SOCK_DGRAM

class udpsend():
  def __init__(self):

    self.HOST = ''
    self.sendPORT = 9180
    self.sendADDRESS = "127.0.1.1"

    self.send = socket(AF_INET, SOCK_DGRAM)
    self.view3Send = struct.Struct("!BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB")
    #                           [00][01][02][03][04][05][06][07][08]
    # self.view3Send = [11,12,13,14, 21,22,23,24, 31,32,33,34, 41,42,43,44,
    #                   51,52,53,54, 61,62,63,64, 71,72,73,74, 81,82,83,84]
    self.view3Send = [00,00,00,00, 00,00,00,00, 00,00,00,00, 00,00,00,00,
                      00,00,00,00, 00,00,00,00, 00,00,00,00, 00,00,00,00]
    self.data = []


  def sender(self):
    s = struct.Struct("!BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB")
    self.data =[00, 36, 00, 00];

    for i in range(4, 9 * 4, 4):
      if (self.view3Send[i - 4] < 0):
        self.view3Send[i - 4] = ((-self.view3Send[i - 4]) ^ 0xffffffff) + 1
      if (self.view3Send[i - 4] > 255):
        self.view3Send[i - 1] = (self.view3Send[i - 4] & 0xff000000) >> 24
        self.view3Send[i - 2] = (self.view3Send[i - 4] & 0x00ff0000) >> 16
        self.view3Send[i - 3] = (self.view3Send[i - 4] & 0x0000ff00) >>  8
        self.view3Send[i - 4] = (self.view3Send[i - 4] & 0x000000ff)

    for i in range(4, 9 * 4):
      # print(i)
      self.data.append(self.view3Send[i - 4]);

    self.data[3] = 0;
    checkSum = 0;
    for i in range(36):
      checkSum += self.data[i];
      if (checkSum > 0xff):
        # print("checkSum = ", checkSum);
        checkSum -= 0x100;
        # print(", after = ", checkSum, end="")
    self.data[3] = (0xff - checkSum);

    # print(data)
    # print(len(data))
    tmpData = []
    for i in range(0, len(self.data), 4):
      tmpData.append(self.data[i + 0])
      tmpData.append(self.data[i + 1])
      tmpData.append(self.data[i + 2])
      tmpData.append(self.data[i + 3])
    self.data = tmpData
    sendData =s.pack(*self.data)
    self.send.sendto(sendData, (self.sendADDRESS, self.sendPORT))
    print("send:", len(self.data), "(" , self.sendADDRESS, ",", self.sendPORT, ")", end="")
    for i in range(len(self.data)):
      print(format(self.data[i], '02x'), end="")
    print()

  def closer(self):
    self.send.close()

udp = udpsend()
while True:
  udp.view3Send[0] += 1
  if (udp.view3Send[0] > 255):
    udp.view3Send[0] = 0
  udp.sender()
  time.sleep(0.1)
udp.closer()


