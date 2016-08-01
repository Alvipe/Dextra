#include <Synapse.h>

int ledPin = 13;
Synapse junction;

void setup() {
    pinMode(ledPin, OUTPUT);
    digitalWrite(13, LOW);
    Serial.begin(115200);
}

void loop() {
    float* setPointArray;
    float positionArray[6] = {10.0,11.1,12.2,13.3,14.4,15.5};
    while(!Serial.available());
    char command = Serial.read();
    switch(command) {
        case 's':
            setPointArray = junction.getSetPoints();
            if(setPointArray[0]==10.0 && setPointArray[1]==11.1 && setPointArray[2]==12.2 && setPointArray[3]==13.3 && setPointArray[4]==14.4 && setPointArray[5]==15.5) {
                digitalWrite(ledPin, HIGH);
            }
            else if(setPointArray[0]==10.0 && setPointArray[1]==0.0 && setPointArray[2]==0.0 && setPointArray[3]==0.0 && setPointArray[4]==0.0 && setPointArray[5]==0.0) {
                digitalWrite(ledPin, LOW);
            }
            break;
        case 'r':
            junction.write(positionArray);
            break;
    }
}
