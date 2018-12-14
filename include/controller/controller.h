#ifndef CONTROLLERSERVICE_H
#define CONTROLLERSERVICE_H

#include <QMutex>
#include <QWaitCondition>
#include <QThread>
#include <sys/time.h>

#include "include/hardware/optoelectroniccontroller.h"
#include "include/hardware/illuminatcontroller.h"
#include "include/hardware/poecamera.h"
#include "include/hardware/cframeinfo.h"
#include "include/config/configservice.h"
#include "include/algorithm/algorithmwrapper.hpp"

enum CAMERA_THREAD_STATUS {
    CAMERA_THREAD_IDLE = 0,
    CAMERA_THREAD_RUNNING
};

enum CAMERA_STATION_STATUS {
    STATION_ONE = 1,
    STATION_TWO,
    STATION_THREE,
    STATION_THREE_ONE,
    STATION_THREE_TWO,
    STATION_THREE_THREE
};

enum CAMERA_SAVE_TYPE {
    CAMERA_SAVE_TYPE_NIR = 0,
    CAMERA_SAVE_TYPE_COLOR
};

enum IMAGE_BUFFER_TYPE {
    IMAGE_BUFFER_PING = 0,
    IMAGE_BUFFER_PONG
};

class CameraThread;

class EventListener {
public:
    virtual void onFramesFromControl(int stationid, QMap<int, CFrameInfo *> framelist) = 0;
    virtual void onFrameFromControl(int stationid, int cameraid, CFrameInfo *frame) = 0;
    virtual void onAnalysisResult(AnalysisResult &result) = 0;
};

class ControllerService : public CameraListener, public AlgorithmListener
{
public:
    ControllerService();
    ~ControllerService();
    static ControllerService *instance;
    static ControllerService *getInstance();
    void addEventListener(EventListener *);
    void savePic(int cameraid, CFrameInfo *frm, QString path);
    bool initAll(STATIONINFO station_info, ALGOINFO algorithm_info);
    void takePicture();
    ICamera *getCamera(int id);
    BYTE *getImageBuf(int id, int ping_pong);
    //callback from Camera
    void onFrame(int cameraid, int stationid, CFrameInfo *);
    //callback from algorithm wrapper
    void postResultFromAlgWrapper(AnalysisResult &result);

    EventListener *mEventListener;
    QVector<IlluminantController *> mIlluControllerlist;
    AlgorithmWrapper *mAlgWrapper;

private:
    QVector<OptoelectronicController *> mPilotControllerlist;
    ICamera *mCamera;
    CameraThread *mCameraThread;
    QVector<ICamera *> mCameraList;
    QVector<BYTE *> mImageBufPing;
    QVector<BYTE *> mImageBufPong;
    bool mContrStatus;
};

class CameraThread :public QThread
{
public:
    explicit CameraThread(ControllerService *instance, int id, int num);
    void takePicture();
    void run();

private:
    ControllerService *parent;
    QMutex mWaitMutex;
    QWaitCondition mWaitCond;
    int mCameraId;
    int mStationId;
    int mCameraNum;
    bool isStop;
    int mStatus;
    int mCurStation;
    int mCurImagebuf[6] = {0};
    CFrameInfo *frame[6];
};
#endif // CONTROLLERSERVICE_H
