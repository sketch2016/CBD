#ifndef ICAMERA_HPP
#define ICAMERA_HPP
#include <QMap>
#include "include/hardware/cframeinfo.h"

enum CAMERA_EVENT {
    CAMERA_INIT_OK,
    CAMERA_INIT_FAIL,
    CAMERA_HEAT_WIRE_OPEN_FAIL,
    CAMERA_DISCONNECTED,
    CAMERA_CONNECTED
};

enum CAMERA_STATUS {
    CAMERA_OK,
    CAMERA_ERROR
};

class CameraListener {
public:
    virtual void onFrame(int cameraid, int stationid, CFrameInfo* frameInfo) = 0;
    //virtual void onEvent(int cameraid,int event,void *) = 0;
};

class ICamera
{
public:
    ICamera();
    virtual bool CameraInit(void) = 0;
    virtual bool CameraDeinit(void) = 0;
    virtual bool CameraEnumate(QString camera_serial) = 0;
    virtual bool CameraOpen(void) = 0;
    virtual bool CameraClose(void) = 0;
    virtual bool CameraStart(bool continuemode, BYTE *bufPing, BYTE *bufPong, int width, int height) = 0;
    virtual bool CameraStop(void) = 0;
    virtual void setListener(CameraListener *) = 0;
    virtual bool getMonoCameraImage(int id, CFrameInfo *frameInfo) = 0;
    virtual void SaveImage(int cameraid, CFrameInfo *frameInfo, QString path) = 0;
    virtual CFrameInfo *getCFrameInfo() = 0;

    void setStatus(int status);
    int getStatus();
    void setCameraId(int id);
    int getCameraId();
    void setStationId(int id);


protected:
    int mCameraId;
    int mStationId;
    int mStatus;
    int isGrabbingFlag;
};

#endif // ICAMERA_HPP
