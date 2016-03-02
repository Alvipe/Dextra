#include <Servo.h>
#include <FlexiTimer2.h>
#include <SeCo.h>

Servo abductor;
SeCo dataLink;

typedef struct {
    int encoderPinA, encoderPinB, phase, enable;
    volatile double encoderCount = 0, angularPos = 0, position = 0;
    float error = 0, control = 0, lastError = 0, lastPosition = 0, setPoint = 0;
} finger;

finger thumb;
finger index;
finger middle;
finger ring;
finger pinky;

#define Kp 1000
#define Kd 0
#define sampleT 10
#define winderRadius 4.5
#define angularRes 2*3.14159/6000 //2*pi/number of encoder tics per motor revolution

int abAngle = map(10, 0, 90, 20, 110);
//char ctrlStr[7];

void setup() {
    thumb.encoderPinA = 3;
    thumb.encoderPinB = 22;
    thumb.phase = 27;
    thumb.enable = 9;
    index.encoderPinA = 2;
    index.encoderPinB = 23;
    index.phase = 28;
    index.enable = 8;
    middle.encoderPinA = 18;
    middle.encoderPinB = 24;
    middle.phase = 29;
    middle.enable = 7;
    ring.encoderPinA = 19;
    ring.encoderPinB = 25;
    ring.phase = 30;
    ring.enable = 6;
    pinky.encoderPinA = 20;
    pinky.encoderPinB = 26;
    pinky.phase = 31;
    pinky.enable = 5;

    pinMode(thumb.encoderPinA, INPUT);
    pinMode(thumb.encoderPinB, INPUT);
    pinMode(thumb.phase, OUTPUT);
    pinMode(thumb.enable, OUTPUT);
    pinMode(index.encoderPinA, INPUT);
    pinMode(index.encoderPinB, INPUT);
    pinMode(index.phase, OUTPUT);
    pinMode(index.enable, OUTPUT);
    pinMode(middle.encoderPinA, INPUT);
    pinMode(middle.encoderPinB, INPUT);
    pinMode(middle.phase, OUTPUT);
    pinMode(middle.enable, OUTPUT);
    pinMode(ring.encoderPinA, INPUT);
    pinMode(ring.encoderPinB, INPUT);
    pinMode(ring.phase, OUTPUT);
    pinMode(ring.enable, OUTPUT);
    pinMode(pinky.encoderPinA, INPUT);
    pinMode(pinky.encoderPinB, INPUT);
    pinMode(pinky.phase, OUTPUT);
    pinMode(pinky.enable, OUTPUT);

    abductor.attach(12);
    abductor.write(abAngle);

    pinMode(13, OUTPUT);

    attachInterrupt(digitalPinToInterrupt(thumb.encoderPinA), thumb_readEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(index.encoderPinA), index_readEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(middle.encoderPinA), middle_readEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ring.encoderPinA), ring_readEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(pinky.encoderPinA), pinky_readEncoder, CHANGE);

    FlexiTimer2::set(sampleT,move_thumb);
    FlexiTimer2::start();

    Serial.begin (115200);
}

void thumb_readEncoder() {
    if (digitalRead(thumb.encoderPinA) == HIGH) {
        if (digitalRead(thumb.encoderPinB) == LOW) {
            thumb.encoderCount = thumb.encoderCount + 1;
        }
        else {
            thumb.encoderCount = thumb.encoderCount - 1;
        }
    }
    else {
        if (digitalRead(thumb.encoderPinB) == HIGH) {
            thumb.encoderCount = thumb.encoderCount + 1;
        }
        else {
            thumb.encoderCount = thumb.encoderCount - 1;
        }
    }
    thumb.angularPos = angularRes*thumb.encoderCount;
    thumb.position = thumb.angularPos*winderRadius;
}

void index_readEncoder() {
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

void middle_readEncoder() {
    if (digitalRead(middle.encoderPinA) == HIGH) {
        if (digitalRead(middle.encoderPinB) == LOW) {
            middle.encoderCount = middle.encoderCount + 1;
        }
        else {
            middle.encoderCount = middle.encoderCount - 1;
        }
    }
    else {
        if (digitalRead(middle.encoderPinB) == HIGH) {
            middle.encoderCount = middle.encoderCount + 1;
        }
        else {
            middle.encoderCount = middle.encoderCount - 1;
        }
    }
    middle.angularPos = angularRes*middle.encoderCount;
    middle.position = middle.angularPos*winderRadius;
}

void ring_readEncoder() {
    if (digitalRead(ring.encoderPinA) == HIGH) {
        if (digitalRead(ring.encoderPinB) == LOW) {
            ring.encoderCount = ring.encoderCount + 1;
        }
        else {
            ring.encoderCount = ring.encoderCount - 1;
        }
    }
    else {
        if (digitalRead(ring.encoderPinB) == HIGH) {
            ring.encoderCount = ring.encoderCount + 1;
        }
        else {
            ring.encoderCount = ring.encoderCount - 1;
        }
    }
    ring.angularPos = angularRes*ring.encoderCount;
    ring.position = ring.angularPos*winderRadius;
}

void pinky_readEncoder() {
    if (digitalRead(pinky.encoderPinA) == HIGH) {
        if (digitalRead(pinky.encoderPinB) == LOW) {
            pinky.encoderCount = pinky.encoderCount + 1;
        }
        else {
            pinky.encoderCount = pinky.encoderCount - 1;
        }
    }
    else {
        if (digitalRead(pinky.encoderPinB) == HIGH) {
            pinky.encoderCount = pinky.encoderCount + 1;
        }
        else {
            pinky.encoderCount = pinky.encoderCount - 1;
        }
    }
    pinky.angularPos = angularRes*pinky.encoderCount;
    pinky.position = pinky.angularPos*winderRadius;
}

void hand_control() {
    move_thumb();
    move_index();
    move_middle();
    move_ring();
    move_pinky();
}

void move_thumb() {
    thumb.error = thumb.setPoint - thumb.position;
    thumb.control = Kp*thumb.error + Kd*(thumb.error-thumb.lastError);
    if (thumb.control >= 0) {
        thumb.control = constrain(thumb.control,0,255);
        digitalWrite(thumb.phase, HIGH);
        if (thumb.control > 100) analogWrite(thumb.enable, thumb.control);
        else analogWrite(thumb.enable, 0);
    }
    else {
        thumb.control = constrain(abs(thumb.control),0,255);
        digitalWrite(thumb.phase, LOW);
        if (thumb.control > 100) analogWrite(thumb.enable, thumb.control);
        else analogWrite(thumb.enable, 0);
    }
    thumb.lastError = thumb.error;
    thumb.lastPosition = thumb.position;
}

void move_index() {
    index.error = index.setPoint - index.position;
    index.control = Kp*index.error + Kd*(index.error-index.lastError);
    if (index.control >= 0) {
        index.control = constrain(index.control,0,255);
        digitalWrite(index.phase, HIGH);
        if (index.control > 100) analogWrite(index.enable, index.control);
        else analogWrite(index.enable, 0);
    }
    else {
        index.control = constrain(abs(index.control),0,255);
        digitalWrite(index.phase, LOW);
        if (index.control > 100) analogWrite(index.enable, index.control);
        else analogWrite(index.enable, 0);
    }
    index.lastError = index.error;
    index.lastPosition = index.position;
}

void move_middle() {
    middle.error = middle.setPoint - middle.position;
    middle.control = Kp*middle.error + Kd*(middle.error-middle.lastError);
    if (middle.control >= 0) {
        middle.control = constrain(middle.control,0,255);
        digitalWrite(middle.phase, HIGH);
        if (middle.control > 100) analogWrite(middle.enable, middle.control);
        else analogWrite(middle.enable, 0);
    }
    else {
        middle.control = constrain(abs(middle.control),0,255);
        digitalWrite(middle.phase, LOW);
        if (middle.control > 100) analogWrite(middle.enable, middle.control);
        else analogWrite(middle.enable, 0);
    }
    middle.lastError = middle.error;
    middle.lastPosition = middle.position;
}

void move_ring() {
    ring.error = ring.setPoint - ring.position;
    ring.control = Kp*ring.error + Kd*(ring.error-ring.lastError);
    if (ring.control >= 0) {
        ring.control = constrain(ring.control,0,255);
        digitalWrite(ring.phase, HIGH);
        if (ring.control > 100) analogWrite(ring.enable, ring.control);
        else analogWrite(ring.enable, 0);
    }
    else {
        ring.control = constrain(abs(ring.control),0,255);
        digitalWrite(ring.phase, LOW);
        if (ring.control > 100) analogWrite(ring.enable, ring.control);
        else analogWrite(ring.enable, 0);
    }
    ring.lastError = ring.error;
    ring.lastPosition = ring.position;
}

void move_pinky() {
    pinky.error = pinky.setPoint - pinky.position;
    pinky.control = Kp*pinky.error + Kd*(pinky.error-pinky.lastError);
    if (pinky.control >= 0) {
        pinky.control = constrain(pinky.control,0,255);
        digitalWrite(pinky.phase, HIGH);
        if (pinky.control > 100) analogWrite(pinky.enable, pinky.control);
        else analogWrite(pinky.enable, 0);
    }
    else {
        pinky.control = constrain(abs(pinky.control),0,255);
        digitalWrite(pinky.phase, LOW);
        if (pinky.control > 100) analogWrite(pinky.enable, pinky.control);
        else analogWrite(pinky.enable, 0);
    }
    pinky.lastError = pinky.error;
    pinky.lastPosition = pinky.position;
}

void loop(){
    while(!Serial.available());
    char ctrlStr = Serial.read();
    switch(ctrlStr) {
        case 't':
            FlexiTimer2::set(sampleT,move_thumb);
            FlexiTimer2::start();
            thumb.setPoint = dataLink.receive();
            break;
        case 'i':
            FlexiTimer2::set(sampleT,move_index);
            FlexiTimer2::start();
            index.setPoint = dataLink.receive();
            break;
        case 'm':
            FlexiTimer2::set(sampleT,move_middle);
            FlexiTimer2::start();
            middle.setPoint = dataLink.receive();
            break;
        case 'r':
            FlexiTimer2::set(sampleT,move_ring);
            FlexiTimer2::start();
            ring.setPoint = dataLink.receive();
            break;
        case 'p':
            FlexiTimer2::set(sampleT,move_pinky);
            FlexiTimer2::start();
            pinky.setPoint = dataLink.receive();
            break;
        default:
            FlexiTimer2::stop();
            break;
        }
    // thumb.setPoint = dataLink.receive();
    // thumb.setPoint = 0;
    // Serial.print(thumb.position);
    // Serial.print('/');
    // Serial.print(thumb.control);
    // Serial.println();
}
