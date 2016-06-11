#include <MsTimer2.h>

// ACS712 current sensor variables
static const double adcRef = 3.273; // ADC reference voltage (Teensy power supply)
static const int bits = 16; // ADC resolution in bits
static const long adcRes = pow(2,bits); // ADC resolution in samples (2^bits)
static const double adcConv = adcRef/adcRes; // ADC sample-to-voltage conversion factor
static const double acsSensitivity = 0.185; // ACS712 sensitivity (185 mV/A)
int currSensPin = A10;
int currentRaw = 0;
double currentIn = 0, currentInPrev = 0, maxCurrent = 0, minCurrent = 0, baseline = 0;
int sampleOk = 0;

// Motor pins and variables
int phase = 4, enable =5;
int dCycle = map(100, 0, 100, 0, 255);
boolean closed, opened;

// Control variables
static const int Kp = 100, Ki = 0, Kd = 0;
float force = 0, setPoint = 300;
float error = 0, control = 0, lastError = 0, lastForce = 0;
int pwmControl = 0;

char command;
int i = 0, cal = 0;

void sampling() {
    currentRaw = analogRead(currSensPin);
    sampleOk = 1;
}

void calibration(unsigned int meanSamples)
{
    while(i < meanSamples) {
        delay(1); // Without the delay, this code doesn't catch the sampling flags
        if(sampleOk == 1) {
            sampleOk = 0;
            i++;
            baseline = baseline + currentRaw*adcConv;
        }
    }
    i = 0;
    baseline = baseline/meanSamples;
}

void getCurrent() {
    if(sampleOk == 1) {
        sampleOk = 0;
        // forcePID();
        currentIn = (currentRaw*adcConv-baseline)/acsSensitivity*1000; // Measured current in mA
        // currentIn = (currentIn + currentInPrev)/2;
        // currentInPrev =  currentIn;
        force = currentIn;
        if(currentIn > maxCurrent) maxCurrent = currentIn;
        if(currentIn < minCurrent) minCurrent = currentIn;
        // Serial.println(maxCurrent,3);
        // Serial.println(minCurrent,3);
        Serial.print(currentIn,3);
        Serial.print(" // ");
        Serial.print(control);
        Serial.print(" // ");
        Serial.println(pwmControl);
    }
}

void forcePID() {
    error = setPoint - force;
    control = Kp*error + Kd*(error-lastError);
    lastError = error;
    lastForce = force;
    if (control >= 0) {
        pwmControl = constrain(control,0,255);
        digitalWrite(phase, HIGH);
        if (pwmControl > 25) {
            analogWrite(enable, pwmControl);
        }
        else {
            analogWrite(enable, 0);
        }
    }
    else {
        pwmControl = constrain(abs(control),0,255);
        digitalWrite(phase, LOW);
        if (pwmControl > 25) {
            analogWrite(enable, pwmControl);
        }
        else {
            analogWrite(enable, 0);
        }
    }
}

void closeIndex() {
    digitalWrite(phase, HIGH);
    analogWrite(enable, dCycle);
    delay(10);
    analogWrite(enable,0);
    // if(!closed && opened) {
    //     digitalWrite(phase, HIGH);
    //     analogWrite(enable, dCycle);
    //     delay(400);
    //     closed = true;
    //     opened = false;
    // }
    // else {
    //     analogWrite(enable,0);
    // }
}

void openIndex() {
    digitalWrite(phase, LOW);
    analogWrite(enable, dCycle);
    delay(10);
    analogWrite(enable,0);
    // if(closed && !opened) {
    //     digitalWrite(phase, LOW);
    //     analogWrite(enable, dCycle);
    //     delay(230);
    //     closed = false;
    //     opened = true;
    // }
    // else {
    //     analogWrite(enable,0);
    // }
}

void setup() {
    pinMode(phase, OUTPUT);
    pinMode(enable, OUTPUT);
    pinMode(currentIn, INPUT);
    closed = false;
    opened = true;
    // analogReference(INTERNAL); // set analog reference to 1.2V internal ref
    analogReadRes(bits); // Teensy 3.1: set ADC resolution to this many bits
    MsTimer2::set(10,sampling);
    MsTimer2::start();
    Serial.begin(115200);
}

void loop() {
    calibration(1000);
    Serial.print("Calibration complete. Baseline voltage = ");
    Serial.println(baseline);
    while(1) {
        getCurrent();
        while(Serial.available()) {
            command = Serial.read();
        }
        switch(command) {
            case 'w':
                openIndex();
                break;
            case 's':
                forcePID();
                // closeIndex();
                break;
        }
        command = 'x';
    }
}
