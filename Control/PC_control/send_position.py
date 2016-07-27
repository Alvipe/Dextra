# -*- coding: utf-8 -*-
"""
Created on Wed Feb 17 17:09:53 2016

@author: alvaro
"""

import sys
import time
import serial
import struct

deviceName = raw_input('Which microcontroller are you using, (A)rduino or (T)eensy?: ')

if deviceName == 'A':
    portname_start = '/dev/ttyUSB'
elif deviceName == 'T':
    portname_start = '/dev/ttyACM'

# Try opening serial ports /dev/ttyUSB0->9
for i in range (0, 10):
    # Change i to a string
    portnum = str(i)
    # Append "i" to the end of portname_start
    portname_full=''.join([portname_start,portnum])
    # Try to open the serial port with i, if this fails try again with i+1
    try:
        ser = serial.Serial(portname_full, 115200, timeout =1)
        break
    except:
        # If we reach i==9 and no port has been opened, exit and print error message
        if (i==9):
            print "No Serial Port Found"
            sys.exit(0)

#ser.setDTR(False)
#time.sleep(1)
#ser.flushInput()
#ser.setDTR(True)
#time.sleep(5)


def send_setpoint(setpoint):
    header = chr(0xAA)
    footer = chr(0xBB)
    check = 0x00
    data = struct.pack('f',float(setpoint))

    for i in range(len(data)):
        check = check^ord(data[i])
    check = chr(check)
    message = header+data+check+footer
    ser.write(message)

while(1):
    fingerName = raw_input('Which finger do you want to move?\n(thumb = t, index = i, middle = m, ring = r, pinky = p): ')
    ser.write(fingerName);
    setpoint = float(raw_input('Enter setpoint: '))
    if(setpoint > 22):
        setpoint = 22
    send_setpoint(setpoint)
