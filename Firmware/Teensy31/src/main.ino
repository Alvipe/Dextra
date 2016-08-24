#include <Arduino.h>
#include <Finger.h>
#include <Synapse.h>
#include <Servo.h>
#include <MsTimer2.h>

#define pidTime 10 //ms

Finger thumb(11,10,15,14);
Finger indx(8,9,17,16);
Finger middle(7,6,19,18);
Finger ring(5,4,21,20);
Finger pinky(2,3,23,22);
Servo abductor;
Synapse dataLink;

float abductorAngle = map(10, 0, 90, 20, 110);

void handControl() {
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
    abductor.write(abductorAngle);
    MsTimer2::set(pidTime, handControl);
    MsTimer2::start();
    Serial.begin(115200);
    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);
}

void loop() {
    float* setPointArray = dataLink.getSetPoints();
    if(setPointArray[2]==5.0) {
        digitalWrite(13, HIGH);
    }
    else {
        digitalWrite(13, LOW);
    }
    // delay(100);
    abductorAngle = map(setPointArray[0], 0, 90, 20, 110);
    abductor.write(abductorAngle);
    thumb.move(setPointArray[1]);
    indx.move(setPointArray[2]);
    middle.move(setPointArray[3]);
    ring.move(setPointArray[4]);
    pinky.move(setPointArray[5]);
}
