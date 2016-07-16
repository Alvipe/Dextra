#ifndef Finger_h
#define Finger_h

#include <Arduino.h>

class Finger {
    public:
        Finger(int phase, int enable, int encoderPinA, int encoderPinB);
        void readEncoder();
        void positionPID();
        void move(float desiredPosition);
        float getPos(char mode);
        int _encoderPinA, _encoderPinB, _phase, _enable;
    private:
        volatile int encoderCount = 0;
        volatile float angularPos = 0, position = 0, setPoint = 0;
        volatile float error = 0, control = 0, lastError = 0, lastPosition = 0;
        int pwmControl = 0;
};

#endif
