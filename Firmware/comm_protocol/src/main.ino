static const uint8_t header = 0xAA;
static const uint8_t footer = 0xBB;
static const uint8_t finger_address[6] = {0x01,0x02,0x03,0x04,0x05,0x06};
static const unsigned int dataSize = 4;
static const unsigned int messageSize = 30;

typedef union {
    float floating;
    uint8_t binary[dataSize];
} binaryFloat;

uint8_t check = 0x00;

bool waitHeader() {
    uint8_t rec;
    while(Serial.available()>0) {
        rec = Serial.read();
        // delayMicroseconds(50);
        if(rec==header) {
            return true;
        }
        else return false;
    }
}

// bool checkMsg(uint8_t *buff) {
//     uint8_t xorChk = 0x00;
//     int i;
//     for(i=0;i<4;i++) {
//         xorChk = xorChk^buff[i];
//     }
//     if(xorChk==check) return true;
//     else return false;
// }

uint8_t *getMessage() {
    static uint8_t message[messageSize];
    uint8_t rec;
    unsigned int i = 0;
    while(Serial.available()>0) {
        rec = Serial.read();
        // delayMicroseconds(50);
        if((rec!=footer)&&(i<messageSize)) {
            message[i] = rec;
            i++;
        }
        else if(rec==footer) {
            return message;
        }
        else return NULL;
    }
}

float *getSetPoints() {
    uint8_t *message;
    binaryFloat data;
    static float setPointArray[6];
    int i = 0;
    while(!waitHeader()) {}
    message = getMessage();
    while(i<messageSize) {
        for(int j=0;j<6;j++) {
            if(message[i]==finger_address[j]) {
                i++;
                for(int k=0;k<dataSize;k++) {
                    data.binary[k] = message[i];
                    i++;
                }
                setPointArray[j] = data.floating;
            }
        }
    }
    return setPointArray;
}

// uint8_t *getData() {
//     static uint8_t inBuff[dataSize];
//     while(Serial.available()>0) {
//         for(int i = 0;i<dataSize;i++) {
//             inBuff[i] = Serial.read();
//             delayMicroseconds(50);
//         }
//     }
//     return inBuff;
// }

// float *getSetPoints() {
//     static float setPointArray[6];
//     uint8_t *inBuff;
//     uint8_t rec;
//     binaryFloat data;
//     while(!waitHeader()) {}
//     for(int i=0;i<6;i++) {
//         rec = Serial.read();
//         delayMicroseconds(50);
//         if(rec == finger_address[i]) {
//             inBuff = getData();
//             for(int j=0;j<dataSize;j++) {
//                 data.binary[j] = inBuff[j];
//             }
//             setPointArray[i] = data.floating;
//         }
//         else if(rec == footer) {
//             return setPointArray;
//         }
//     }
// }

void setup() {
    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);
    Serial.begin(115200);
}

void loop() {
    float *setPointArray = getSetPoints();
    if(setPointArray[0]==10.0 && setPointArray[1]==11.1 && setPointArray[2]==12.2 && setPointArray[3]==13.3 && setPointArray[4]==14.4 && setPointArray[5]==15.5) {
        digitalWrite(13, HIGH);
    }
    else {
        digitalWrite(13, LOW);
    }
    // for(int i=0;i<6;i++) {
    //     Serial.print(setPointArray[i]);
    //     Serial.print("||");
    // }
    // Serial.println();

}
