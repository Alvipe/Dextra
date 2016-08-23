from kivy.app import App
from kivy.uix.tabbedpanel import TabbedPanel
from kivy.properties import ObjectProperty, ListProperty
from kivy.clock import Clock

import sys
import time
import serial
import struct

header = chr(0x7E)
footer = chr(0x7E)
finger_address = [chr(0x01),chr(0x02),chr(0x03),chr(0x04),chr(0x05),chr(0x06)]

class RootWidget(TabbedPanel):
    ser = None
    scheduled_event = None
    setpoint_list = ListProperty([10.0, 0.0, 0.0, 0.0, 0.0, 0.0])
    serial_port_list = ListProperty([])
    devices = ObjectProperty(None)
    status = ObjectProperty(None)

    def scan_ports(self):
        self.devices.text = "Select device"
        self.serial_port_list = []
        portname_start = ["/dev/ttyUSB","/dev/ttyACM","COM"]
        for port in portname_start:
            for i in range(10):
                portnum = str(i)
                portname_full = ''.join([port,portnum])
                try:
                    self.ser = serial.Serial(portname_full, 115200, timeout=1)
                    self.ser.close()
                    self.serial_port_list.append(portname_full)
                except:
                    self.ser = None

    def connect_to_serial(self,serial_port):
        try:
            self.ser = serial.Serial(serial_port, 115200, timeout=1)
            self.ser.setDTR(False)
            time.sleep(1)
            self.ser.flushInput()
            self.ser.setDTR(True)
            time.sleep(2)
            self.status.text = "Connected"
            self.status.color = [0,1,0,1]
        except:
            self.ser = None
            self.status.text = "No serial port found"
            self.status.color = [1,0,0,1]
            print("No serial port found")

    def connect_press(self):
        self.connect_to_serial(self.devices.text)
        if(self.ser):
            self.scheduled_event = Clock.schedule_interval(self.send_setpoint_list, 0.1)
    def disconnect_press(self):
        if(self.ser):
            self.ser.close()
            Clock.unschedule(self.scheduled_event)
            # Clock.unschedule(self.send_setpoint_list) # METHOD NOT RECOMMENDED
            self.ser = None
            self.status.text = "Disconnected"
            self.status.color = [1,1,0,1]
        else:
            self.status.text = "Not connected"
            self.status.color = [1,0,0,1]
    def send_setpoint_list(self,dt):
        message = header
        for i in range(len(self.setpoint_list)):
            data = struct.pack('f',float(self.setpoint_list[i]))
            message += finger_address[i] + data
        message += footer
        try:
            self.ser.write(message)
            print self.setpoint_list
        except:
            Clock.unschedule(self.scheduled_event)
            print "Stopped"

class DextraControlApp(App):
    def build(self):
        return RootWidget()

if __name__ == '__main__':
    DextraControlApp().run()
