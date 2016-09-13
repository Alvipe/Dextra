import serial
import time
import struct

HEADER = chr(0x7E)
FOOTER = chr(0x7E)
FINGER_ADDRESS = [chr(0x01),chr(0x02),chr(0x03),chr(0x04),chr(0x05),chr(0x06)]

def scan_ports():
    serial_port_list = []
    portname_start = ["/dev/ttyUSB","/dev/ttyACM","COM"]
    for port in portname_start:
        for i in range(10):
            portnum = str(i)
            portname_full = ''.join([port,portnum])
            try:
                ser = serial.Serial(portname_full, 115200, timeout=1)
                ser.close()
                serial_port_list.append(portname_full)
            except serial.SerialException:
                pass
    return serial_port_list

def connect(port_name):
    try:
        ser = serial.Serial(port_name, 115200, timeout=1)
        ser.setDTR(False)
        time.sleep(1)
        ser.flushInput()
        ser.setDTR(True)
        time.sleep(2)
        status = True
    except serial.SerialException:
        ser = None
        status = False
    return ser, status

def send_setpoint_list(ser,setpoint_list):
    message = HEADER
    for i in range(len(setpoint_list)):
        data = struct.pack('f',float(setpoint_list[i]))
        message += FINGER_ADDRESS[i] + data
    message += FOOTER
    try:
        ser.write(message)
    except serial.SerialException:
        raise

def get_message():
    message = ''
    in_byte = chr(0x00)
    message_size = 30
    i = 0
    while(ser.read()!=HEADER): None
    while(ser.inWaiting()>0):
        in_byte = ser.read()
        if(in_byte!=FOOTER and i<message_size):
            message += in_byte
            i += 1
        elif(in_byte==FOOTER):
            return message

def get_data():
    message = ''
    data_list = []
    i = 0
    message = get_message()
    for j in range(6):
        if(message[i]==FINGER_ADDRESS[j]):
            data = ''
            i += 1
            for k in range(4):
                data += message[i]
                i += 1
            data = round(struct.unpack('f',data)[0],4)
            data_list.append(data)
    return data_list
