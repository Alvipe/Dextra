import serial
import time
import struct

HEADER = chr(0x7E)
FOOTER = chr(0x7E)
FINGER_ADDRESS = [chr(0x01),chr(0x02),chr(0x03),chr(0x04),chr(0x05),chr(0x06)]
FLOAT_SIZE = 4
MESSAGE_SIZE = 30
DATA_POINTS = 6

def scan_serial_ports():
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
                None
    return serial_port_list

def connect_to_serial(port_name):
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

def check_message(message, check):
    xor_check = 0x00
    for i in range(len(message)):
        for j in range(len(message[i])):
            xor_check = xor_check^ord(message[i][j])
    xor_check = chr(xor_check)
    if(xor_check==check):
        return True
    else:
        return False

def get_message(ser):
    message = ''
    in_byte = chr(0x00)
    i = 0
    while(ser.read()!=HEADER): None
    while(ser.inWaiting()>0):
        in_byte = ser.read()
        if(in_byte!=FOOTER and i<MESSAGE_SIZE):
            message += in_byte
            i += 1
        elif(in_byte!=FOOTER and i==MESSAGE_SIZE):
            check = in_byte
        elif(in_byte==FOOTER):
            return message, check

def read_data_list(ser):
    data_list = []
    i = 0
    message, check = get_message(ser)
    for j in range(DATA_POINTS):
        if(message[i]==FINGER_ADDRESS[j]):
            data = ''
            i += 1
            for k in range(FLOAT_SIZE):
                data += message[i]
                i += 1
            data = round(struct.unpack('f',data)[0],4)
            data_list.append(data)
    return data_list

def write_setpoint_list(ser,setpoint_list):
    check = 0x00
    message = HEADER
    for i in range(DATA_POINTS):
        data = struct.pack('f',float(setpoint_list[i]))
        message += FINGER_ADDRESS[i] + data
        check = check^ord(FINGER_ADDRESS[i])
        for j in range(FLOAT_SIZE):
            check = check^ord(data[j])
    check = chr(check)
    message += check + FOOTER
    try:
        ser.write(message)
    except serial.SerialException:
        raise
