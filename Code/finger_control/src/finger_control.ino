#include <Servo.h>
#include <FlexiTimer2.h>
#include <SeCo.h>

Servo abductor;
SeCo dataLink;

#define sampleT 10
#define Kp 1000
#define Kd 0
#define winderRadius 4.5
#define angularRes 2*3.14159/6000 //2*pi/number of encoder tics per motor revolution

int abAngle = map(10, 0, 90, 20, 110);

class finger {
    private:
        int _encoderPinA, _encoderPinB, _phase, _enable;
        volatile float encoderCount = 0, angularPos = 0, position = 0, setPoint = 0;;
        volatile float error = 0, control = 0, lastError = 0, lastPosition = 0;
        int pwmControl = 0;
    public:
        void attach(int phase, int enable, int encoderPinA, int encoderPinB, String name) {
            pinMode(encoderPinA, INPUT);
            pinMode(encoderPinB, INPUT);
            pinMode(phase, OUTPUT);
            pinMode(enable, OUTPUT);
            if (name == "thumb") {
                attachInterrupt(digitalPinToInterrupt(encoderPinA), thumbReadEncoder, CHANGE);
            }
            else if (name == "index") {
                attachInterrupt(digitalPinToInterrupt(encoderPinA), indexReadEncoder, CHANGE);
            }
            else if (name == "middle") {
                attachInterrupt(digitalPinToInterrupt(encoderPinA), middleReadEncoder, CHANGE);
            }
            else if (name == "ring") {
                attachInterrupt(digitalPinToInterrupt(encoderPinA), ringReadEncoder, CHANGE);
            }
            else if (name == "pinky") {
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
            angularPos = angularRes*encoderCount;
            position = angularPos*winderRadius;
        }

        void positionPID() {
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
                    if (pwmControl > 100) {
                        analogWrite(_enable, pwmControl);
                    }
                    else {
                        analogWrite(_enable, 0);
                    }
                }
                else {
                    pwmControl = constrain(abs(control),0,255);
                    digitalWrite(_phase, LOW);
                    if (pwmControl > 100) {
                        analogWrite(_enable, pwmControl);
                    }
                    else {
                        analogWrite(_enable, 0);
                    }
                }
            }
        }

        void write(float desiredPosition) {
            if (desiredPosition > 22) {
                desiredPosition = 22;
            }
            else if (desiredPosition < -12) {
                desiredPosition = -12;
            }
            setPoint = desiredPosition;
        }

        float read() {
            return position;
        }
};

finger thumb;
finger index;
finger middle;
finger ring;
finger pinky;

void hand_control() {
    thumb.positionPID();
    index.positionPID();
    middle.positionPID();
    ring.positionPID();
    pinky.positionPID();
}

void thumbReadEncoder() {
    thumb.readEncoder();
}

void indexReadEncoder() {
    index.readEncoder();
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
    thumb.attach(27,9,3,22,"thumb");
    index.attach(28,8,2,23,"index");
    middle.attach(29,7,18,24,"middle");
    ring.attach(30,6,19,25,"ring");
    pinky.attach(31,5,20,26,"pinky");
    abductor.attach(12);
    abductor.write(abAngle);
    FlexiTimer2::set(sampleT, hand_control);
    FlexiTimer2::start();
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
            index.write(dataLink.receive());
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
