from kivy.app import App
from kivy.uix.tabbedpanel import TabbedPanel
from kivy.uix.boxlayout import BoxLayout
from kivy.properties import ObjectProperty, ListProperty, StringProperty
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
    setpoint_list = [10.0, 0.0, 0.0, 0.0, 0.0, 0.0]
    slider_values = ListProperty(setpoint_list)
    status = StringProperty("Not connected")
    abductor = ObjectProperty(None)
    thumb = ObjectProperty(None)
    index = ObjectProperty(None)
    middle = ObjectProperty(None)
    ring = ObjectProperty(None)
    pinky = ObjectProperty(None)

    def connect_to_serial(self):
        portname_start = ["/dev/ttyUSB","/dev/ttyACM","COM"]
        for port in portname_start:
            for i in range(10):
                portnum = str(i)
                portname_full = ''.join([port,portnum])
                try:
                    self.ser = serial.Serial(portname_full, 115200, timeout=1)
                    self.ser.setDTR(False)
                    time.sleep(1)
                    self.ser.flushInput()
                    self.ser.setDTR(True)
                    time.sleep(2)
                    self.status = "Connected"
                    # return ser
                    break
                except:
                    if(i == 9 and port == "COM"):
                        print("No serial port found")
                        self.status = "No serial port found"
                        # sys.exit(0)

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
    def connect_press(self):
        self.connect_to_serial()
        if(self.ser):
            self.scheduled_event = Clock.schedule_interval(self.send_setpoint_list, 0.1)
    def disconnect_press(self):
        if(self.ser):
            self.ser.close()
            Clock.unschedule(self.scheduled_event)
            # Clock.unschedule(self.send_setpoint_list) # METHOD NOT RECOMMENDED
            self.ser = None
            self.status = "Disconnected"
        else:
            self.status = "Not connected"
    def send_setpoint_list(self,dt):
        message = header
        for i in range(len(self.setpoint_list)):
            data = struct.pack('f',float(self.setpoint_list[i]))
            message += finger_address[i] + data
        message += footer
        if(self.ser.isOpen()):
            self.ser.write(message)
            print self.setpoint_list
        # else:
        #     self.status = "Not connected"

class DextraControlApp(App):
    def build(self):
        # Clock.schedule_interval(RootWidget().send_setpoint_list, 0.1)
        return RootWidget()

if __name__ == '__main__':
    # ser = connect_to_serial()
    DextraControlApp().run()
    # ser.close()
