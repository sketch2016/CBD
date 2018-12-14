#ifndef ILLUMINANTCONTROLLER_H
#define ILLUMINANTCONTROLLER_H

#include "include/hardware/serialport.h"

class IlluminantController
{
public:
    IlluminantController();
    void init(char *dev_name);
    void SetLed1(char brightness);
    void SetLed2(char brightness);
    void SetLed3(char brightness);
    void SetLed4(char brightness);
    void setBrightness(char led1,char led2,char led3,char led4);
    void flush();
    void setTriggerMode(bool mode);

private:
    char mLedLightness1;
    char mLedLightness2;
    char mLedLightness3;
    char mLedLightness4;
    SerialPort *mSerialPort;
};

#endif // ILLUMINANTCONTROLLER_H
