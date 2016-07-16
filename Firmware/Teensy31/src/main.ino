#include <Arduino.h>
#include <Finger.h>
#include <Servo.h>
#include <MsTimer2.h>
#include <SeCo.h>

#define pidTime 10 //ms

Finger thumb(11,10,15,14);
Finger indx(8,9,17,16);
Finger middle(7,6,19,18);
Finger ring(5,4,21,20);
Finger pinky(2,3,23,22);
Servo abductor;
SeCo dataLink;

int abAngle = map(10, 0, 90, 20, 110);

void hand_control() {
    thumb.positionPID();
    indx.positionPID();
    middle.positionPID();
    ring.positionPID();
    pinky.positionPID();
}

void thumbReadEncoder() {
    thumb.readEncoder();
}

void indexReadEncoder() {
    indx.readEncoder();
}

void middleReadEncoder() {
    middle.readEncoder();
}

void ringReadEncoder() {
    ring.readEncoder();
}

void pinkyReadEncoder() {
    pinky.readEncoder();
}

void setup() {
    attachInterrupt(digitalPinToInterrupt(thumb._encoderPinA), thumbReadEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(indx._encoderPinA), indexReadEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(middle._encoderPinA), middleReadEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ring._encoderPinA), ringReadEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(pinky._encoderPinA), pinkyReadEncoder, CHANGE);
    abductor.attach(12);
    abductor.write(abAngle);
    MsTimer2::set(pidTime, hand_control);
    MsTimer2::start();
    Serial.begin(115200);
}

void loop() {
    while(!Serial.available());
    char ctrl = Serial.read();
    switch(ctrl) {
        case 't':
            thumb.move(dataLink.receive());
            break;
        case 'i':
            indx.move(dataLink.receive());
            break;
        case 'm':
            middle.move(dataLink.receive());
            break;
        case 'r':
            ring.move(dataLink.receive());
            break;
        case 'p':
            pinky.move(dataLink.receive());
            break;
    }
}
