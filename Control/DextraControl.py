from kivy.app import App
from kivy.uix.tabbedpanel import TabbedPanel
from kivy.properties import ObjectProperty, ListProperty
from kivy.clock import Clock

import synapse

class RootWidget(TabbedPanel):
    ser = None
    scheduled_event = None
    setpoint_list = ListProperty([10.0, 0.0, 0.0, 0.0, 0.0, 0.0])
    serial_port_list = ListProperty([])
    devices = ObjectProperty(None)
    status = ObjectProperty(None)

    def scan_ports(self):
        if not self.status.value:
            self.devices.text = "Select device"
            self.serial_port_list = synapse.scan_ports()

    def connect(self):
        if not self.status.value:
            self.ser, self.status.value = synapse.connect(self.devices.text)
            if self.status.value:
                self.scheduled_event = Clock.schedule_interval(self.send_setpoint_list, 0.1)
                self.status.text = "Connected"
                self.status.color = [0,1,0,1]
            else:
                self.status.text = "No serial port found"
                self.status.color = [1,0,0,1]

    def disconnect(self):
        if self.status.value:
            self.ser.close()
            Clock.unschedule(self.scheduled_event)
            self.status.value = False
            self.status.text = "Disconnected"
            self.status.color = [1,1,0,1]
        else:
            self.status.text = "Not connected"
            self.status.color = [1,0,0,1]

    def send_setpoint_list(self,dt):
        try:
            synapse.send_setpoint_list(self.ser,self.setpoint_list)
            # print self.setpoint_list
        except:
            Clock.unschedule(self.scheduled_event)
            self.status.value = False
            self.status.text = "Transmission error"
            self.status.color = [1,0,0,1]

class DextraControlApp(App):
    def build(self):
        return RootWidget()

if __name__ == '__main__':
    DextraControlApp().run()
