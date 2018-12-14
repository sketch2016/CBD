#ifndef CAMERA_H
#define CAMERA_H

#include "GenICam/System.h"
#include "GenICam/Camera.h"
#include "GenICam/StreamSource.h"
#include "Media/ImageConvert.h"
//#include "cframeinfo.h"
#include "include/hardware/icamera.h"
#include <QString>

typedef unsigned char BYTE;
#define IPCONFIG_PATH "./config_file/ip_config.txt"

class PoeCamera:public ICamera
{
public:
    explicit PoeCamera();
    ~PoeCamera();

    bool CameraInit(void);
    bool CameraDeinit(void);
    //enumate camera
    bool CameraEnumate(QString camera_serial);
    //build one connect
    bool CameraOpen(void);
    //remove one connect
    bool CameraClose(void);
    //build streamsource and start to grab
    bool CameraStart(bool continuemode, BYTE *bufPing, BYTE *bufPong, int width, int height);
    //stop to grab and reset streamsource
    bool CameraStop(void);
    void setListener(CameraListener *listener);
    void SaveImage(int cameraid, CFrameInfo* frameInfo, QString path);
    bool getMonoCameraImage(int id, CFrameInfo* frameInfo);
    CFrameInfo *getCFrameInfo();

    void SaveColorImage(CFrameInfo* frameInfo, QString path);
    bool setExposeTime(double exposureTime);
    bool setGrabMode(bool bContious);
    bool setTriggerDelay(double dDelayTime);
    bool CameraStartGrabbing();
    bool CameraRegGetFrameCallback();
    bool CameraUnRegGetFrameCallback();
    void onGetFrame(const Dahua::GenICam::CFrame &pFrame);
    //Dahua::GenICam::CSystem &systemObj;

private:
    //all camera list
    Dahua::Infra::TVector<Dahua::GenICam::ICameraPtr> m_vCameraPtrList;
    //color camera list
    Dahua::Infra::TVector<Dahua::GenICam::ICameraPtr> m_vColorCameraPtrList;
    //mono camera list in first station
    Dahua::Infra::TVector<Dahua::GenICam::ICameraPtr> m_vMonoCameraPtrList;
    //stream source
    Dahua::Infra::TVector<Dahua::GenICam::IStreamSourcePtr> m_pMonoStreamSource;

    //build connect
    //bool CameraOpenOne(Dahua::GenICam::ICameraPtr pCamera);
    //remove connect
    //bool CameraCloseOne(Dahua::GenICam::ICameraPtr pCamera);
    bool getframe(Dahua::GenICam::IStreamSourcePtr pStreamSource, CFrameInfo *frameInfo);

    int mCameraNum;
    int m_triggerMode;
    int mCurImageBuf;
    CFrameInfo *frameInfo;
    BYTE *mBufferPing;
    BYTE *mBufferPong;
    CameraListener *mListener;
    Dahua::GenICam::ICameraPtr pCamera;
    Dahua::GenICam::IStreamSourcePtr m_pStreamSource;
};

#endif // CAMERA_H
