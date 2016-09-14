#include "Arduino.h"
#include "Synapse.h"

static const uint8_t header = 0x7E;
static const uint8_t footer = 0x7E;
static const uint8_t fingerAddress[6] = {0x01,0x02,0x03,0x04,0x05,0x06};
static const unsigned int floatSize = 4;
static const unsigned int messageSize = 30; // 6 floats + 6 finger addresses of 1 byte
static const unsigned int dataPoints = 6;

typedef union {
    float floating;
    uint8_t binary[floatSize];
} binaryFloat;

Synapse::Synapse(Stream &serial) {
    _serial = &serial;
}

bool Synapse::waitHeader() {
    uint8_t inByte;
    while(_serial->available()>0) {
        inByte = _serial->read();
        // delayMicroseconds(50);
        if(inByte==header) {
            return true;
        }
        else {
            return false;
        }
    }
    return false;
}

bool Synapse::checkMessage(uint8_t *message) {
    uint8_t xorCheck = 0x00;
    unsigned int i;
    for(i=0;i<messageSize;i++) {
        xorCheck = xorCheck^message[i];
    }
    if(xorCheck==check) {
        return true;
    }
    else {
        return false;
    }
}

void Synapse::getMessage(uint8_t* message) {
    uint8_t inByte;
    unsigned int i=0;
    while(!waitHeader()) {}
    while(_serial->available()>0) {
        inByte = _serial->read();
        // delayMicroseconds(50);
        if((inByte!=footer)&&(i<messageSize)) {
            message[i] = inByte;
            i++;
        }
        else if((inByte!=footer)&&(i==messageSize)) {
            check = inByte;
        }
        else if(inByte==footer) {
            return;
        }
    }
}

void Synapse::readSetPoints(float* setPointArray) {
    uint8_t message[messageSize];
    binaryFloat data;
    unsigned int i=0, j, k;
    getMessage(message);
    for(j=0;j<6;j++) {
        if(message[i]==fingerAddress[j]) {
            i++;
            for(k=0;k<floatSize;k++) {
                data.binary[k] = message[i];
                i++;
            }
            setPointArray[j] = data.floating;
        }
    }
    return;
}

void Synapse::writeDataArray(float* dataArrayOut) {
    binaryFloat data;
    uint8_t message[messageSize+3];
    uint8_t xorCheck = 0x00;
    message[0] = header;
    unsigned int i=1, j, k;
    for(j=0;j<6;j++) {
        data.floating = dataArrayOut[j];
        message[i] = fingerAddress[j];
        i++;
        for(k=0;k<floatSize;k++) {
            message[i] = data.binary[k];
            i++;
        }
    }
    message[messageSize+1] = xorCheck;
    message[messageSize+2] = footer;
    _serial->write(message,messageSize+3);
}
