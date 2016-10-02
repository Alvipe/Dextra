#include <MsTimer2.h>
#include <Synapse.h>

static const unsigned int EMG_CH1 = 0;
static const unsigned int ledPin = 13;
static const unsigned int sampleTime = 1;
static const double adcRef= 5.00;
static const unsigned int adcRes = 1023;
static const double adcConv = adcRef/adcRes;

volatile unsigned int emg = 0;
double emgMean = 0.00;
double emgMovav = 0.00;
double emgMvc = 0.00;
volatile unsigned int sOK = 0;
unsigned int i = 0, k = 0;

float setpointArray[6] = {10.0,0.0,0.0,0.0,0.0,0.0};

Synapse dataLink(Serial);

/* sampling reads the ADC every 1 ms with the MsTimer2 interrupt. */
void sampling()
{
    emg = analogRead(EMG_CH1);
    sOK = 1; // sOK indicates that there is a new sample
}

/* meanCalc computes the mean value of the EMG signal during a period of
meanSamples. The mean value of the signal is its baseline. This value is
subtracted from the measured EMG signals to have a baseline of 0 V. */
void meanCalc(unsigned int meanSamples)
{
    while(i < meanSamples)
    {
        delayMicroseconds(50);
        if(sOK==1)
        {
            sOK = 0;
            i++;
            emgMean = emgMean + emg*adcConv;
        }
    }
    i = 0;
    emgMean = emgMean/meanSamples;
}

/* movAv computes the running moving average of the EMG signal. First, the
baseline of the signal is lowered to 0 V, to be able to rectify it. The running
moving average rectifies and smooths the signal (acts as a low pass filter),
returning the amplitude of the signal. */
void movAv()
{
    double emgZero = emg*adcConv - emgMean; // Signal with 0 V baseline
    emgMovav = emgMovav*0.99 + abs(emgZero)*0.01; // Rectified and smoothed signal
}

/* mvcCalc computes the maximum voluntary contraction, the maximum force the
user is able to exert. This value is used to compute the activation threshold */
void mvcCalc(unsigned int mvcSamples)
{
    while(i < mvcSamples)
    {
        delayMicroseconds(50);
        if(sOK==1)
        {
            sOK = 0;
            i++;
            movAv();
            if(emgMovav > emgMvc) {
                emgMvc = emgMovav;
            }
        }
    }
    i = 0;
    emgMovav = 0.00;
}

/* blinkLED blinks a led "repeat" times with a "bTime" interval between on and off */
void blinkLED(unsigned int repeat,unsigned int bTime)
{
    for(i=0;i<repeat;i++)
    {
        digitalWrite(ledPin,HIGH);
        delay(bTime);
        digitalWrite(ledPin,LOW);
        delay(bTime);
    }
}

void setup() {
    pinMode(EMG_CH1,INPUT);
    pinMode(ledPin,OUTPUT);
    pinMode(12,OUTPUT);
    MsTimer2::set(sampleTime, sampling);
    MsTimer2::start();
    Serial.begin(115200);

    /* System calibration */
    delay(5000);
    /* Calibration step #1: calculate the baseline of the signal during 10 s */
    blinkLED(1,500); // LED indicates calibration step #1 start
    meanCalc(10000);
    blinkLED(1,500); // LED indicates calibration step #1 end
    delay(1000);
    /* Calibration step #2: calculate the maximum voluntary contraction during 5 s*/
    blinkLED(2,500); // LED indicates calibration step #2 start
    mvcCalc(5000);
    blinkLED(2,500); // LED indicates calibration step #2 end
}

void loop() {
    delayMicroseconds(50);
    if(sOK) {
        sOK = 0;
        movAv(); // Gets the amplitude of the measured EMG signal

        /* If the amplitude of the EMG signal is greater that a 35% of the MVC
        (indicating a voluntary muscle contraction), the finger position values
        are set to a closing position.*/
        if(emgMovav > 0.35*emgMvc) {
            // MsTimer2::stop();
            // setpointArray[0] = 10.0;
            // setpointArray[1] = 0.0;
            // setpointArray[2] = 15.0;
            // setpointArray[3] = 15.0;
            // setpointArray[4] = 15.0;
            // setpointArray[5] = 15.0;
            digitalWrite(ledPin, HIGH);
            digitalWrite(12,HIGH);
            // delay(100);
            // MsTimer2::start();
        }
        else {
            // MsTimer2::stop();
            // setpointArray[0] = 10.0;
            // setpointArray[1] = 0.0;
            // setpointArray[2] = 0.0;
            // setpointArray[3] = 0.0;
            // setpointArray[4] = 0.0;
            // setpointArray[5] = 0.0;
            digitalWrite(ledPin, LOW);
            digitalWrite(12,LOW);
            // delay(100);
            // MsTimer2::start();
        }
    }
    // MsTimer2::stop();
    // dataLink.writeDataArray(&setpointArray[0]);
    // MsTimer2::start();
    // delay(2);
    // Serial.println(emgMovav);
}
