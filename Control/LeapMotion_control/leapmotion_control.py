# -*- coding: utf-8 -*-
"""
Created on Thu Jul 14 16:12:25 2016

@author: alvaro
"""

import os, sys, inspect
src_dir = os.path.dirname(inspect.getfile(inspect.currentframe()))
arch_dir = 'lib/x64' if sys.maxsize > 2**32 else 'lib/x86'
sys.path.insert(0, os.path.abspath(os.path.join(src_dir, arch_dir)))

import Leap
import numpy as np

#angles = {}
#phalanges = {}
finger_names = ['thumb','index','middle','ring','pinky']
bone_names = ['metacarpal','proximal','intermediate','distal']

class SampleListener(Leap.Listener):

    def on_connect(self, controller):
        print "Connected"

    def on_frame(self, controller):
        frame = controller.frame()
        right_hand = frame.hands.rightmost
        
#        for i,finger in enumerate(finger_names):
#            for j,phalanx in enumerate(bone_names):
#                phalanges[phalanx] = right_hand.fingers[i].bone(j)
#            angle_MCP = phalanges['proximal'].direction.angle_to(phalanges['metacarpal'].direction)*(180/np.pi)
#            angle_PIP = phalanges['intermediate'].direction.angle_to(phalanges['proximal'].direction)*(180/np.pi)
#            angle_DIP = phalanges['distal'].direction.angle_to(phalanges['intermediate'].direction)*(180/np.pi)
#            angles[finger] = [angle_MCP,angle_PIP,angle_DIP]
#        print (np.sum(angles['thumb']),np.sum(angles['index']),np.sum(angles['middle']),np.sum(angles['ring']),np.sum(angles['pinky']))
##        print hand
        
        angles = []
        for i in range(5):
            phalanges = []
            for j in range(4):
                phalanges.append(right_hand.fingers[i].bone(j))
            angle_MCP = phalanges[1].direction.angle_to(phalanges[0].direction)*(180/np.pi)
            angle_PIP = phalanges[2].direction.angle_to(phalanges[1].direction)*(180/np.pi)
            angle_DIP = phalanges[3].direction.angle_to(phalanges[2].direction)*(180/np.pi)
            angles.append([angle_MCP,angle_PIP,angle_DIP])
        print np.sum(angles[0]),np.sum(angles[1]),np.sum(angles[2]),np.sum(angles[3]),np.sum(angles[4])
#        print angles

def main():
    # Create a sample listener and controller
    listener = SampleListener()
    controller = Leap.Controller()

    # Have the sample listener receive events from the controller
    controller.add_listener(listener)

    # Keep this process running until Enter is pressed
    print "Press Enter to quit..."
    try:
        sys.stdin.readline()
    except KeyboardInterrupt:
        pass
    finally:
        # Remove the sample listener when done
        controller.remove_listener(listener)

if __name__ == "__main__":
    main()