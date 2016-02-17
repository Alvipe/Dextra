#include <FlexiTimer2.h>
#include <SeCo.h>

SeCo dataLink;

#define encoderPinA  2
#define encoderPinB  3

#define Kp 1000
#define Kd 0
#define sampleT 1

#define winderRadius 4.5

int phase = 4;
int enable = 5;
int dCycle = map(20, 0, 100, 0, 255);
int flag;

double angularRes = 2*3.14159/6000; //2*pi/number of encoder tics per motor revolution
volatile double encoderCount = 0, angularPos = 0, position = 0;
double error = 0, control = 0;
double lastError = 0, lastPosition = 0;
float setPoint = 0;

void setup() {
    pinMode(encoderPinA, INPUT);
    pinMode(encoderPinB, INPUT);

    pinMode(phase, OUTPUT);
    pinMode(enable, OUTPUT);
    pinMode(13, OUTPUT);
    flag = 0;

    attachInterrupt(digitalPinToInterrupt(encoderPinA), readEncoder, CHANGE);

    FlexiTimer2::set(sampleT,control_loop);
    FlexiTimer2::start();

    Serial.begin (115200);
}

void readEncoder() {
    if (digitalRead(encoderPinA) == HIGH) {
        if (digitalRead(encoderPinB) == LOW) {
            encoderCount = encoderCount + 1;
        }
        else {
            encoderCount = encoderCount - 1;
        }
    }

    else {
        if (digitalRead(encoderPinB) == HIGH) {
            encoderCount = encoderCount + 1;
        }
        else {
            encoderCount = encoderCount - 1;
        }
    }
    angularPos = angularRes*encoderCount;
    position = angularPos*winderRadius;
}

void control_loop() {
    error = setPoint-position;
    control = Kp*error + Kd*(error-lastError);
    if (control >= 0) {
        control = constrain(control,0,255);
        digitalWrite(phase, HIGH);
        analogWrite(enable, control);
    }
    else {
        control = constrain(abs(control),0,255);
        digitalWrite(phase, LOW);
        analogWrite(enable, control);
    }
    lastError = error;
    lastPosition = position;
}

void loop(){
    setPoint = dataLink.receive();
}
