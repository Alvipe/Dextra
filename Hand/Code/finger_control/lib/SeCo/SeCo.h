#ifndef SeCo_h
#define SeCo_h

#include <Arduino.h>

class SeCo {
    public:
        float receive();
        void transmit(float dataToSend);
    private:
        boolean waitHeader();
        byte* inMsg();
        boolean checkMsg(byte *buff);
};

#endif
