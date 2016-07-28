# -*- coding: utf-8 -*-
"""
Created on Wed Jul 27 02:05:48 2016

@author: alvaro
"""

import sys
import time
import serial
import struct

if hasattr(__builtins__, 'raw_input'):
    input = raw_input

portname_start = ["/dev/ttyUSB","/dev/ttyACM","COM"]
for port in portname_start:
    for i in range(10):
        portnum = str(i)
        portname_full = ''.join([port,portnum])
        try:
            ser = serial.Serial(portname_full, 115200, timeout=1)
            break
        except:
            if(i == 9 and port == "COM"):
                print("No serial port found")
                sys.exit(0)

#deviceName = input("Which microcontroller are you using, (A)rduino or (T)eensy?: ")
#
#if deviceName == 'A':
#    portname_start = '/dev/ttyUSB'
#elif deviceName == 'T':
#    portname_start = '/dev/ttyACM'
#
## Try opening serial ports /dev/ttyUSB0->9
#for i in range (0, 10):
#    # Change i to a string
#    portnum = str(i)
#    # Append "i" to the end of portname_start
#    portname_full=''.join([portname_start,portnum])
#    # Try to open the serial port with i, if this fails try again with i+1
#    try:
#        ser = serial.Serial(portname_full, 115200, timeout =1)
#        break
#    except:
#        # If we reach i==9 and no port has been opened, exit and print error message
#        if (i==9):
#            print("No Serial Port Found")
#            sys.exit(0)
#
#ser = serial.Serial('COM4', 115200, timeout=1)

#ser.setDTR(False)
#time.sleep(1)
#ser.flushInput()
#ser.setDTR(True)
#time.sleep(5)

def send_setpoint_list(setPointList):
    data = []
    finger_address = [chr(0x01),chr(0x02),chr(0x03),chr(0x04),chr(0x05),chr(0x06)]
    header = chr(0xAA)
    footer = chr(0xBB)
    check = 0x00
    for i in range(len(setPointList)):
        data.append(struct.pack('f',float(setPointList[i])))
    message = header
    for j in range(len(setPointList)):
        message += finger_address[j] + data[j]
        for k in range(len(data[j])):
            check = check^ord(data[j][k])
    check = chr(check)
    message += footer
#    message += check + footer
    ser.write(message)
    return message

def getMessage():
    header = chr(0xAA)
    footer = chr(0xBB)
    message = []
    rec = chr(0x00)
    messageSize = 30
    i = 0
    while(ser.read()!=header): None
    while(ser.inWaiting()>0):
        rec = ser.read()
        if(rec!=footer and i<messageSize):
            message.append(rec)
            i += 1
        elif(rec==footer):
            return message

def getData():
    message = []
    data = 0.0
    dataList = []
    finger_address = [chr(0x01),chr(0x02),chr(0x03),chr(0x04),chr(0x05),chr(0x06)]
    # i = 0
    message = getMessage()
    for i in range(6):
        if(message[i]==finger_address[i]):
            data = round(struct.unpack('f',message[i])[0],4)
            dataList.append(data)
            print("Oh yeah!")
        else:
            print("Ohhh :_(")

while(1):
    # action = input("What do you want to do, (c)lose or (o)pen the hand?: ")
    # if action == 'c':
    #     setPointList = [10.0,11.1,12.2,13.3,14.4,15.5]
    # elif action == 'o':
    #     setPointList = [10.0,0.0,0.0,0.0,0.0,0.0]
    # message = send_setpoint_list(setPointList)
    getData()
