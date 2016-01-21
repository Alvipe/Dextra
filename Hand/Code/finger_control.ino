#include <Servo.h>

Servo abductor;
int phasePin = 6;
int enablePin = 7;
int dCycle = map(60, 0, 100, 0, 255);
int abAngle = map(10, 0, 90, 20, 110);
char control;
int i;
int flag;

void setup() {
    pinMode(phasePin, OUTPUT);
    pinMode(enablePin, OUTPUT);
    abductor.attach(3);
    abductor.write(abAngle);
    Serial.begin(115200);
}

void loop() {
    while(Serial.available()) {
        control = Serial.read();
        flag = 0;
    }
    if(flag == 0) {
        switch(control) {
            case 'o':
                openFinger();
                break;
            case 'c':
                closeFinger();
                break;
            case '1':
                abAngle = map(10, 0, 90, 20, 110);
                abductor.write(abAngle);
                break;
            case '2':
                abAngle = map(45, 0, 90, 20, 110);
                abductor.write(abAngle);
                break;
            case '3':
                abAngle = map(90, 0, 90, 20, 110);
                abductor.write(abAngle);
                break;
        }
    }
    else {
        analogWrite(phasePin,0);
    }
}

void closeFinger() {
    digitalWrite(enablePin, HIGH);
    for(i=0;i<10;i++) {
        analogWrite(phasePin, dCycle);
        delay(100);
    }
    flag = 1;
}

void openFinger() {
    digitalWrite(enablePin, LOW);
    for(i=4;i>0;i--) {
        analogWrite(phasePin, dCycle);
        delay(100);
    }
    flag = 1;
}

