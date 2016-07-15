#include <Arduino.h>
#include <Servo.h>
#include <MsTimer2.h>
#include <SeCo.h>

Servo abductor;
SeCo dataLink;

#define pidTime 10 //ms
#define Kp 1000
#define Kd 0
#define winderRadius 4.5
#define angularRes 2*3.14159/6000 //2*pi/number of encoder tics per motor revolution

int abAngle = map(10, 0, 90, 20, 110);

class finger {
private:
    int _encoderPinA, _encoderPinB, _phase, _enable;
    volatile int encoderCount = 0;
    volatile float angularPos = 0, position = 0, setPoint = 0;
    volatile float error = 0, control = 0, lastError = 0, lastPosition = 0;
    int pwmControl = 0;
public:
    void attach(int phase, int enable, int encoderPinA, int encoderPinB, char name) {
        pinMode(encoderPinA, INPUT);
        digitalWrite(encoderPinA, HIGH);
        pinMode(encoderPinB, INPUT);
        digitalWrite(encoderPinA, HIGH);
        pinMode(phase, OUTPUT);
        pinMode(enable, OUTPUT);
        if (name == 't') {
            attachInterrupt(digitalPinToInterrupt(encoderPinA), thumbReadEncoder, CHANGE);
        }
        else if (name == 'i') {
            attachInterrupt(digitalPinToInterrupt(encoderPinA), indexReadEncoder, CHANGE);
        }
        else if (name == 'm') {
            attachInterrupt(digitalPinToInterrupt(encoderPinA), middleReadEncoder, CHANGE);
        }
        else if (name == 'r') {
            attachInterrupt(digitalPinToInterrupt(encoderPinA), ringReadEncoder, CHANGE);
        }
        else if (name == 'p') {
            attachInterrupt(digitalPinToInterrupt(encoderPinA), pinkyReadEncoder, CHANGE);
        }
        _phase = phase;
        _enable = enable;
        _encoderPinA = encoderPinA;
        _encoderPinB = encoderPinB;
    }

    void readEncoder() {
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

    void positionPID() {
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
                digitalWrite(_phase, HIGH);
                if (pwmControl > 25) {
                    analogWrite(_enable, pwmControl);
                }
                else {
                    analogWrite(_enable, 0);
                }
            }
            else {
                pwmControl = constrain(abs(control),0,255);
                digitalWrite(_phase, LOW);
                if (pwmControl > 25) {
                    analogWrite(_enable, pwmControl);
                }
                else {
                    analogWrite(_enable, 0);
                }
            }
        }
    }

    void write(float desiredPosition) {
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

    float read(char mode) {
        cli();
        if (mode == 'e') return encoderCount;
        else if (mode == 'a') return angularPos;
        else if (mode == 'p') return position;
        sei();
    }
};

finger thumb;
finger indx;
finger middle;
finger ring;
finger pinky;

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
    thumb.attach(11,10,15,14,'t');
    indx.attach(8,9,17,16,'i');
    middle.attach(7,6,19,18,'m');
    ring.attach(5,4,21,20,'r');
    pinky.attach(2,3,23,22,'p');
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
            thumb.write(dataLink.receive());
            break;
        case 'i':
            indx.write(dataLink.receive());
            break;
        case 'm':
            middle.write(dataLink.receive());
            break;
        case 'r':
            ring.write(dataLink.receive());
            break;
        case 'p':
            pinky.write(dataLink.receive());
            break;
    }
}
