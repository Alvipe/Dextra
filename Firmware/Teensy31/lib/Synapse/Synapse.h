#ifndef Synapse_h
#define Synapse_h

#include <Arduino.h>

class Synapse {
    public:
        float *getSetPoints();
        void write(float* messageToSend);
    private:
        bool waitHeader();
        bool checkMsg(uint8_t *buff);
        uint8_t* getMessage();
};

#endif
