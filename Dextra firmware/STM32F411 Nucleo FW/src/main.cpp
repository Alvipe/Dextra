#include "mbed.h"

static const float pidTime = 0.01; // 10 ms
static const int Kp = 5;
static const int Kd = 0;
static const double spoolRadius = 4.5; // 4.5 mm
static const double angularRes = 2*3.14159/6000; //2*pi/number of encoder tics per motor revolution

Ticker controller;

class Finger {
private:
    DigitalOut _phase;
    PwmOut _enable;
    DigitalIn _encoderPinA;
    DigitalIn _encoderPinB;
    InterruptIn _encoderInterrupt;
    volatile int encoderCount = 0;
    volatile double angularPos = 0, position = 0, setPoint = 0;
    volatile double control = 0, error = 0, lastError = 0, lastPosition = 0;
    int pwmControl = 0;
public:
    Finger(PinName phase, PinName enable, PinName encoderPinA, PinName encoderPinB) : _phase(phase),_enable(enable),_encoderPinA(encoderPinA),_encoderPinB(encoderPinB), _encoderInterrupt(encoderPinA) {
        _encoderInterrupt.rise(this, &Finger::readEncoder);
        _encoderInterrupt.fall(this, &Finger::readEncoder);
        _enable.period_us(20); // Set PWM frequency to 50 KHz
    }

    void readEncoder() {
        if (_encoderPinA.read() == 1) {
            if (_encoderPinB.read() == 0) {
                encoderCount = encoderCount + 1;
            } else {
                encoderCount = encoderCount - 1;
            }
        } else {
            if (_encoderPinB.read() == 1) {
                encoderCount = encoderCount + 1;
            } else {
                encoderCount = encoderCount - 1;
            }
        }
    }

    void positionPID() {
        angularPos = angularRes*encoderCount;
        position = angularPos*spoolRadius;
        error = setPoint - position;
        control = Kp*error + Kd*(error-lastError);
        lastError = error;
        lastPosition = position;
        if ((position > 22)||(position < -12)) {
            _enable.write(0.00f);
        } else {
            if (control >= 0.00f) {
                if (control > 1.00f) {
                    pwmControl = 1.00f;
                } else {
                    pwmControl = control;
                }
                _phase.write(1);
                if (pwmControl > 0.00f) {
                    _enable.write(pwmControl);
                } else {
                    _enable.write(0.00f);
                }
            } else {
                if (fabs(control) > 1.00f) {
                    pwmControl = 1.00f;
                } else {
                    pwmControl = fabs(control);
                }
                _phase.write(0);
                if (pwmControl > 0.00f) {
                    _enable.write(pwmControl);
                } else {
                    _enable.write(0.00f);
                }
            }
        }
    }

    void setPos(double desiredPosition) {
        __disable_irq();
        if (desiredPosition > 22) {
            desiredPosition = 22;
        } else if (desiredPosition < -12) {
            desiredPosition = -12;
        }
        setPoint = desiredPosition;
        __enable_irq();
    }

    float readPos(char mode) {
        __disable_irq();
        if (mode == 'e') {
            return encoderCount;
        } else if (mode == 'a') {
            return angularPos;
        } else if (mode == 'l') {
            return position;
        }
        __enable_irq();
    }
};

// Finger thumb(PA_2,PA_3,PA_4,PA_5);
Finger indx(PA_10,PB_3,PB_5,PB_4);
Finger middle(PB_10,PA_8,PA_9,PC_7);
// Finger ring(PA_2,PA_3,PA_4,PA_5);
// Finger pinky(PA_2,PA_3,PA_4,PA_5);

void hand_control() {
    // thumb.positionPID();
    indx.positionPID();
    middle.positionPID();
    // ring.positionPID();
    // pinky.positionPID();
}

Serial pc(SERIAL_TX, SERIAL_RX);

void setup() {
    // abductor.attach(12);
    // abductor.write(abAngle);
    controller.attach(&hand_control, pidTime);
    pc.baud(115200);
}

int main() {
    setup();
    while(1) {
        // thumb.setPos(0);
        indx.setPos(0);
        middle.setPos(0);
        pc.printf("%f\n", indx.readPos('l'));
        // ring.setPos(0);
        // pinky.setPos(0);
        wait_ms(3000);
        // thumb.setPos(0);
        indx.setPos(15);
        middle.setPos(15);
        pc.printf("%f\n", indx.readPos('l'));
        // ring.setPos(15);
        // pinky.setPos(15);
        wait_ms(3000);
    }
}
