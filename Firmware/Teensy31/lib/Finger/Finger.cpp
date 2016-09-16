#include <Arduino.h>
#include <Finger.h>

#define Kp 1000
#define Kd 0
#define winderRadius 4.5
#define angularRes 2*3.14159/6000 //2*pi/number of encoder tics per motor revolution

Finger::Finger(int phase, int enable, int encoderPinA, int encoderPinB) {
    pinMode(encoderPinA, INPUT);
    digitalWrite(encoderPinA, HIGH);
    pinMode(encoderPinB, INPUT);
    digitalWrite(encoderPinB, HIGH);
    pinMode(phase, OUTPUT);
    pinMode(enable, OUTPUT);
    _phase = phase;
    _enable = enable;
    _encoderPinA = encoderPinA;
    _encoderPinB = encoderPinB;
}

void Finger::readEncoder() {
    if (digitalRead(_encoderPinA) == HIGH) {
        if (digitalRead(_encoderPinB) == LOW) {
            encoderCount = encoderCount + 1;
        }
        else {
            encoderCount = encoderCount - 1;
        }
    }
    else {
        if (digitalRead(_encoderPinB) == HIGH) {
            encoderCount = encoderCount + 1;
        }
        else {
            encoderCount = encoderCount - 1;
        }
    }
}

void Finger::positionPID() {
    angularPos = angularRes*encoderCount;
    position = angularPos*winderRadius;
    error = setPoint - position;
    control = Kp*error + Kd*(error-lastError);
    lastError = error;
    lastPosition = position;
    if ((position > 22)||(position < -12)) {
        analogWrite(_enable, 0);
    }
    else {
        if (control >= 0) {
            pwmControl = constrain(control,0,255);
            digitalWrite(_phase, LOW);
            if (pwmControl > 25) {
                analogWrite(_enable, pwmControl);
            }
            else {
                analogWrite(_enable, 0);
            }
        }
        else {
            pwmControl = constrain(abs(control),0,255);
            digitalWrite(_phase, HIGH);
            if (pwmControl > 25) {
                analogWrite(_enable, pwmControl);
            }
            else {
                analogWrite(_enable, 0);
            }
        }
    }
}

void Finger::writePosition(float desiredPosition) {
    cli();
    if (desiredPosition > 22) {
        desiredPosition = 22;
    }
    else if (desiredPosition < -12) {
        desiredPosition = -12;
    }
    setPoint = desiredPosition;
    sei();
}

float Finger::readPosition(char mode) {
    cli();
    if (mode == 'e') return encoderCount;
    else if (mode == 'a') return angularPos;
    else if (mode == 'p') return position;
    sei();
}
