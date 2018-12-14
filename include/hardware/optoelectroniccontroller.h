#ifndef PILOTLAMPCONTROLLER_H
#define PILOTLAMPCONTROLLER_H

#include "include/hardware/serialport.h"

class OptoelectronicController
{
public:
    OptoelectronicController();
    void turnOn();
    void turnOff();

private:
    SerialPort *mSerialPort;
};

#endif // PILOTLAMPCONTROLLER_H
