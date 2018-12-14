#ifndef SERIALPORT_H
#define SERIALPORT_H


class SerialPort
{
public:
    SerialPort(char *dev,int mBoundrate);
    int listenPort(char * data);
    int writeData(void *data,int len);
    void closePort();

private:
    int mBoudrate;
    int mFd;
};

#endif // SERIALPORT_H
