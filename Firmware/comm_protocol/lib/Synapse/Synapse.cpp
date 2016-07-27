#include "Arduino.h"
#include "Synapse.h"

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

bool Synapse::waitHeader() {
    uint8_t rec;
    while(Serial.available()>0) {
        rec = Serial.read();
        // delayMicroseconds(50);
        if(rec==header) {
            return true;
        }
        else return false;
    }
    return false;
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

uint8_t* Synapse::getMessage() {
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
    return NULL;
}

float* Synapse::getSetPoints() {
    uint8_t* message;
    binaryFloat data;
    static float setPointArray[6];
    unsigned int i = 0;
    while(!waitHeader()) {}
    message = getMessage();
    while(i<messageSize) {
        for(unsigned int j=0;j<6;j++) {
            if(message[i]==finger_address[j]) {
                i++;
                for(unsigned int k=0;k<dataSize;k++) {
                    data.binary[k] = message[i];
                    i++;
                }
                setPointArray[j] = data.floating;
            }
        }
    }
    return setPointArray;
}

// uint8_t* Synapse::getData() {
//     static uint8_t inBuff[dataSize];
//     while(Serial.available()>0) {
//         for(int i = 0;i<dataSize;i++) {
//             inBuff[i] = Serial.read();
//             delayMicroseconds(50);
//         }
//     }
//     return inBuff;
// }

// float* Synapse::getSetPoints() {
//     static float setPointArray[6];
//     uint8_t* inBuff;
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
