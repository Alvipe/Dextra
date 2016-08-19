from kivy.app import App
from kivy.uix.boxlayout import BoxLayout
from kivy.properties import ObjectProperty, ListProperty
from kivy.clock import Clock

import sys
import time
import serial
import struct

header = chr(0x7E)
footer = chr(0x7E)
finger_address = [chr(0x01),chr(0x02),chr(0x03),chr(0x04),chr(0x05),chr(0x06)]

def connect_to_serial():
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
    ser.setDTR(False)
    time.sleep(1)
    ser.flushInput()
    ser.setDTR(True)
    time.sleep(2)
    return ser

class RootWidget(BoxLayout):
    setpoint_list = [10.0, 0.0, 0.0, 0.0, 0.0, 0.0]
    slider_values = ListProperty(setpoint_list)
    abductor = ObjectProperty(None)
    thumb = ObjectProperty(None)
    index = ObjectProperty(None)
    middle = ObjectProperty(None)
    ring = ObjectProperty(None)
    pinky = ObjectProperty(None)

    def updateAbductor(self):
        self.setpoint_list[0] = self.abductor.value
    def updateThumb(self):
        self.setpoint_list[1] = self.thumb.value
    def updateIndex(self):
        self.setpoint_list[2] = self.index.value
    def updateMiddle(self):
        self.setpoint_list[3] = self.middle.value
    def updateRing(self):
        self.setpoint_list[4] = self.ring.value
    def updatePinky(self):
        self.setpoint_list[5] = self.pinky.value
    def send_setpoint_list(self,dt):
        message = header
        for i in range(len(self.setpoint_list)):
            data = struct.pack('f',float(self.setpoint_list[i]))
            message += finger_address[i] + data
        message += footer
        ser.write(message)
        print self.setpoint_list

class DextraControlApp(App):
    def build(self):
        Clock.schedule_interval(RootWidget().send_setpoint_list, 0.1)
        return RootWidget()

if __name__ == '__main__':
    ser = connect_to_serial()
    DextraControlApp().run()
    ser.close()
