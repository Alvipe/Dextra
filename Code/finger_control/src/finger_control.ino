#include <FlexiTimer2.h>
#include <SeCo.h>

SeCo dataLink;

typedef struct {
    int encoderPinA, encoderPinB, phase, enable;
    volatile double encoderCount = 0, angularPos = 0, position = 0;
    float error = 0, control = 0, lastError = 0, lastPosition = 0, setPoint = 0;
} finger;

finger index;

#define Kp 1000
#define Kd 0
#define sampleT 1
#define winderRadius 4.5
#define angularRes 2*3.14159/6000 //2*pi/number of encoder tics per motor revolution

void setup() {
    index.encoderPinA = 2;
    index.encoderPinB = 3;
    index.phase = 4;
    index.enable = 5;

    pinMode(index.encoderPinA, INPUT);
    pinMode(index.encoderPinB, INPUT);
    pinMode(index.phase, OUTPUT);
    pinMode(index.enable, OUTPUT);

    pinMode(13, OUTPUT);

    attachInterrupt(digitalPinToInterrupt(index.encoderPinA), readEncoder, CHANGE);

    FlexiTimer2::set(sampleT,control_loop);
    FlexiTimer2::start();

    Serial.begin (115200);
}

void readEncoder() {
    if (digitalRead(index.encoderPinA) == HIGH) {
        if (digitalRead(index.encoderPinB) == LOW) {
            index.encoderCount = index.encoderCount + 1;
        }
        else {
            index.encoderCount = index.encoderCount - 1;
        }
    }

    else {
        if (digitalRead(index.encoderPinB) == HIGH) {
            index.encoderCount = index.encoderCount + 1;
        }
        else {
            index.encoderCount = index.encoderCount - 1;
        }
    }
    index.angularPos = angularRes*index.encoderCount;
    index.position = index.angularPos*winderRadius;
}

void control_loop() {
    index.error = index.setPoint - index.position;
    index.control = Kp*index.error + Kd*(index.error-index.lastError);
    if (index.control >= 0) {
        index.control = constrain(index.control,0,255);
        digitalWrite(index.phase, HIGH);
        analogWrite(index.enable, index.control);
    }
    else {
        index.control = constrain(abs(index.control),0,255);
        digitalWrite(index.phase, LOW);
        analogWrite(index.enable, index.control);
    }
    index.lastError = index.error;
    index.lastPosition = index.position;
}

void loop(){
    index.setPoint = dataLink.receive();
}
