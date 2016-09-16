#include <Arduino.h>
#include <Finger.h>
#include <Synapse.h>
#include <Servo.h>
#include <MsTimer2.h>

#define pidTime 10 //ms

Finger thumb(2,4,23,22);
Finger indx(7,5,21,20);
Finger middle(8,6,19,18);
Finger ring(11,9,17,16);
Finger pinky(12,10,15,14);
Servo abductor;
Synapse dataLink(Serial);

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
    abductor.attach(3);
    abductor.write(abductorAngle);
    MsTimer2::set(pidTime, handControl);
    MsTimer2::start();
    Serial.begin(115200);
    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);
}

void loop() {
    float setPointArray[6];
    dataLink.readSetPoints(&setPointArray[0]);
    // delay(100);
    abductorAngle = map(setPointArray[0], 0, 90, 20, 110);
    abductor.write(abductorAngle);
    thumb.writePosition(setPointArray[1]);
    indx.writePosition(setPointArray[2]);
    middle.writePosition(setPointArray[3]);
    ring.writePosition(setPointArray[4]);
    pinky.writePosition(setPointArray[5]);
}
