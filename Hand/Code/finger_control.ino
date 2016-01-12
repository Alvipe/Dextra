int phasePin = 6;
int enablePin = 7;
int dCycle = map(40, 0, 100, 0, 255);
char control;
int i;
int flag;

void setup() {
    pinMode(phasePin, OUTPUT);
    pinMode(enablePin, OUTPUT);
    Serial.begin(115200);
}

void loop() {
    while(Serial.available()) {
        control = Serial.read();
        flag = 0;
    }
    if(flag == 0) {
        if(control=='o') {
            openFinger();
        }
        else if(control=='c') {
            closeFinger();
        }
    }
    else {
        analogWrite(phasePin,0);
    }
}

void closeFinger() {
    digitalWrite(enablePin, HIGH);
    for(i=0;i<14;i++) {
        analogWrite(phasePin, dCycle);
        delay(100);
    }
    flag = 1;
}

void openFinger() {
    digitalWrite(enablePin, LOW);
    for(i=6;i>0;i--) {
        analogWrite(phasePin, dCycle);
        delay(100);
    }
    flag = 1;
}

