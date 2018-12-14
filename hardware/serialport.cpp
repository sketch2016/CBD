#include "include/hardware/serialport.h"
#include "include/util/logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>

#define TAG "serialport"

SerialPort::SerialPort(char *dev,int boundrate)
{
    int fd = 0;
    struct termios tio;

    /* Open the device to be non-blocking (read will return immediatly) */
    fd = open(dev, O_RDWR | O_NOCTTY);
    LOGD(TAG, "open fd is %1", fd);

    if (fd < 0) {
        return;
    }

    /* Set new port settings */
    tcgetattr(fd, &tio); /* Save current port settings */
    tio.c_cflag = CS8 | CREAD | CLOCAL ;//| CRTSCTS;
    tio.c_iflag = 0;
    tio.c_oflag = 0;
    tio.c_lflag = 0;

    tio.c_cc[VMIN] = 1; /* Block until at least this many chars have been received */
    tio.c_cc[VTIME] = 0; /* Timeout value 100ms */

    cfsetospeed(&tio, boundrate);
    cfsetispeed(&tio, boundrate);
    if (tcsetattr(fd, TCSANOW,&tio) < 0) {
         LOGD(TAG, "open fd fail");
         return;
    }

    tcflush(fd, TCIOFLUSH);
    mFd = fd;
    LOGD(TAG, "open fd finish");
}

int SerialPort::listenPort(char *data) {
    return read(mFd, data, 1);
}

void SerialPort::closePort() {
    close(mFd);
}

int SerialPort::writeData(void *data,int len) {
   LOGD(TAG, "write data len is %1", len);
   return write(mFd,data,len);
}
