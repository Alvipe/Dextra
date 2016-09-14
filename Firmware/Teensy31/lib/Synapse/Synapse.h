#ifndef Synapse_h
#define Synapse_h

#include <Arduino.h>

class Synapse {
    public:
        Synapse(Stream &serial);
        void readSetPoints(float* setPointArray);
        void writeDataArray(float* dataArrayOut);
    private:
        bool waitHeader();
        bool checkMessage(uint8_t* message);
        void getMessage(uint8_t* message);
        Stream* _serial;
        uint8_t check;
};

#endif
