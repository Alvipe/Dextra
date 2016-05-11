#include "Arduino.h"
#include "SeCo.h"

#define header 0xAA
#define footer 0xBB
#define dataSize sizeof(float)

typedef union {
    float floating;
    byte binary[dataSize];
} binaryFloat;

byte check = 0x00;

boolean SeCo::waitHeader() {
    byte rec;
    while(Serial.available()>0) {
        rec = Serial.read();
        delayMicroseconds(100);
        if(rec==header) {
            return true;
        }
        else return false;
    }
}

byte* SeCo::inMsg() {
    byte rec;
    static byte inBuff[4];
    int i = 0;
    while(Serial.available()>0) {
        rec = Serial.read();
        delayMicroseconds(100);
        if((rec!=footer)&&(i<dataSize)) {
            inBuff[i] = rec;
            i++;
            if(i==4) {
                check = Serial.read();
                delayMicroseconds(100);
            }
        }
        if(rec==footer) {
            i = 0;
            return inBuff;
        }
    }
}

boolean SeCo::checkMsg(byte *buff) {
    byte xorChk = 0x00;
    int i;
    for(i=0;i<4;i++) {
        xorChk = xorChk^buff[i];
    }
    if(xorChk==check) return true;
    else return false;
}

float SeCo::receive() {
    byte *inBuff;
    binaryFloat data;
    int i;
    while(!waitHeader()) {}
    inBuff = inMsg();
    if(checkMsg(inBuff)) {
        for(i=0;i<4;i++) {
            data.binary[i] = inBuff[i];
        }
        return data.floating;
    }
}

void SeCo::transmit(float dataToSend) {
    binaryFloat data;
    data.floating = dataToSend;
    byte message[7];
    byte xorChk = 0x00;
    int i, j;
    for(i=0;i<4;i++) {
        xorChk = xorChk^data.binary[i];
    }
    message[0] = header;
    for(j=1;j<5;j++) {
        message[j] = data.binary[j-1];
    }
    message[5] = xorChk;
    message[6] = footer;
    // Serial.write(header);
    // Serial.write(data.binary,4);
    // Serial.write(xorChk);
    // Serial.write(footer);
    Serial.write(message,7);
}
