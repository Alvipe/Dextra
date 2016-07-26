// #include "Arduino.h"
// #include "SeCo.h"

#define header 0xAA
#define footer 0xBB
#define inBuffSize 100

static const uint8_t finger_address[6] = {0x01,0x02,0x03,0x04,0x05,0x06};
static const unsigned int dataSize = 4;
static const unsigned int messageSize = 30;

typedef union {
    float floating;
    uint8_t binary[dataSize];
} binaryFloat;

uint8_t check = 0x00;

// uint8_t *createBuffer(float data, int position) {
//     binaryFloat inData;
//     inData.floating = data;
//     uint8_t binaryBuffer[400];
//     int inc = 4;
//     for(int i=0;i<dataSize;i++) {
//         binaryBuffer[position*inc+i] = inData.binary[i];
//     }
//     return binaryBuffer;
// }

// uint8_t *createBuffer(float *inBuff) {
//     // int inBuffSize = sizeof(inBuff);
//     static uint8_t binaryBuffer[inBuffSize*dataSize];
//     binaryFloat inData;
//     for(int i=0;i<inBuffSize;i++) {
//         inData.floating = inBuff[i];
//         for(int j=0;j<dataSize;j++) {
//             binaryBuffer[i*dataSize+j] = inData.binary[j];
//         }
//     }
//     return binaryBuffer;
// }

bool waitHeader() {
    uint8_t rec;
    while(Serial.available()>0) {
        rec = Serial.read();
        delayMicroseconds(50);
        if(rec==header) {
            return true;
        }
        else return false;
    }
}

uint8_t *getMessage() {
    static uint8_t message[messageSize];
    uint8_t rec;
    unsigned int i = 0;
    while(Serial.available()>0) {
        rec = Serial.read();
        delayMicroseconds(50);
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

// uint8_t *inMsg() {
//     uint8_t rec;
//     static uint8_t inBuff[dataSize];
//     int i = 0;
//     while(Serial.available()>0) {
//         rec = Serial.read();
//         delayMicroseconds(100);
//         if((rec!=footer)&&(i<dataSize)) {
//             inBuff[i] = rec;
//             i++;
//             if(i==4) {
//                 check = Serial.read();
//                 delayMicroseconds(100);
//             }
//         }
//         if(rec==footer) {
//             i = 0;
//             return inBuff;
//         }
//     }
// }

bool checkMsg(uint8_t *buff) {
    uint8_t xorChk = 0x00;
    int i;
    for(i=0;i<4;i++) {
        xorChk = xorChk^buff[i];
    }
    if(xorChk==check) return true;
    else return false;
}

// float *getSetPoints() {
//     static float setPointArray[6];
//     uint8_t *inBuff;
//     uint8_t rec;
//     binaryFloat data;
//     while(waitHeader()) {}
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

// float receive() {
//     uint8_t *inBuff;
//     binaryFloat data;
//     int i;
//     while(!waitHeader()) {}
//     inBuff = inMsg();
//     if(checkMsg(inBuff)) {
//         for(i=0;i<4;i++) {
//             data.binary[i] = inBuff[i];
//         }
//         return data.floating;
//     }
// }

// void transmit(float dataToSend) {
//     binaryFloat data;
//     data.floating = dataToSend;
//     uint8_t message[7];
//     uint8_t xorChk = 0x00;
//     int i, j;
//     for(i=0;i<4;i++) {
//         xorChk = xorChk^data.binary[i];
//     }
//     message[0] = header;
//     for(j=1;j<5;j++) {
//         message[j] = data.binary[j-1];
//     }
//     message[5] = xorChk;
//     message[6] = footer;
//     Serial.write(message,7);
//     // Serial.write(header);
//     // Serial.write(data.binary,4);
//     // Serial.write(xorChk);
//     // Serial.write(footer);
// }

void setup() {
    Serial.begin(115200);
}

void loop() {
    float *setPointArray = getSetPoints();
}
