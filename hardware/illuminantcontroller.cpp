#include <termios.h>
#include "include/util/logger.h"

#include "include/hardware/illuminatcontroller.h"

#define TAG "illuminatcontroller"
/*
*       相机          光源
*工位1	camera1		 光源1
*工位2	camera2		 光源2
*工位3	camera3/4/5	 光源3/4/5/6/7
*工位3上，camera3对应光源3,camera4对应光源4/5, camera5对应光源6/7
*工位3上需要2个电源控制器来分别控制5个光源
*/
IlluminantController::IlluminantController(){
    //mSerialPort = new SerialPort("/dev/ttyS4",B9600);
    mLedLightness1 = 0;
    mLedLightness2 = 0;
    mLedLightness3 = 0;
    mLedLightness4 = 0;
}

void IlluminantController::init(char *dev_name) {
    mSerialPort = new SerialPort(dev_name, B9600);
    setBrightness(0,0,0,0);
}

void IlluminantController::SetLed1(char brightness) {
    mLedLightness1 = brightness;
}

void IlluminantController::SetLed2(char brightness) {
    mLedLightness2 = brightness;
}

void IlluminantController::SetLed3(char brightness) {
    mLedLightness3 = brightness;
}

void IlluminantController::SetLed4(char brightness) {
    mLedLightness4 = brightness;
}

void IlluminantController::flush() {
    setBrightness(mLedLightness1,mLedLightness2,mLedLightness3,mLedLightness4);
}

void IlluminantController::setTriggerMode(bool mode) {
    if (mode)
        LOGD(TAG, "it is trigger mode now!");
}

/**/
void IlluminantController::setBrightness(char led1,char led2,char led3,char led4) {
    unsigned char data[] = {0xAC,led1,led2,led3,led4};
    LOGD(TAG, "setbrightness1: %1, setbrightness2: %2, setbrightness3: %3, setbrightness4: %4", led1,led2,led3,led4);

    int size = mSerialPort->writeData(data,sizeof(data));
    LOGD(TAG, "setBright size is %1", size);
}




