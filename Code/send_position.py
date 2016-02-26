# -*- coding: utf-8 -*-
"""
Created on Wed Feb 17 17:09:53 2016

@author: alvaro
"""

import time
import serial
import struct

ser = serial.Serial('/dev/ttyUSB0',115200,timeout=None)
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
    setpoint = float(raw_input('Enter setpoint: '))
    if(setpoint > 23):
        setpoint = 23
    send_setpoint(setpoint)
