#include <Synapse.h>

int ledPin = 13;
Synapse junction;

void setup() {
    pinMode(ledPin, OUTPUT);
    digitalWrite(13, LOW);
    Serial.begin(115200);
}

void loop() {
    float *setPointArray = junction.getSetPoints();
    if(setPointArray[0]==10.0 && setPointArray[1]==11.1 && setPointArray[2]==12.2 && setPointArray[3]==13.3 && setPointArray[4]==14.4 && setPointArray[5]==15.5) {
        digitalWrite(ledPin, HIGH);
    }
    else {
        digitalWrite(ledPin, LOW);
    }
}
