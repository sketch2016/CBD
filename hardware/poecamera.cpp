#include "include/hardware/poecamera.h"
#include "include/hardware/cframeinfo.h"
#include "include/util/logger.h"
#include <qimage.h>
#include <QFile>

#define TAG "poecamera"

using namespace Dahua::GenICam;
using namespace Dahua::Infra;

PoeCamera::PoeCamera()
{
    isGrabbingFlag = false;
    mCurImageBuf = 0;
    frameInfo = new CFrameInfo();
}

PoeCamera::~PoeCamera()
{
}

bool PoeCamera::getframe(IStreamSourcePtr pStreamSource, CFrameInfo *frameInfo)
{
    CFrame frame;
    if(pStreamSource == nullptr) {
        LOGD(TAG, "camera pStreamSource is null");
    }

    if(frameInfo == nullptr) {
        LOGD(TAG, "camera frame is null");
    }

    bool result = pStreamSource->getFrame(frame, 5000);
    LOGD(TAG, "got one frame from stream source. result is %1", result);

    bool isValid = frame.valid();
    if (!isValid)
    {
        LOGD(TAG, "frame is invalid!");
        return false;
    }

    if (frameInfo->m_nWidth != static_cast<int>(frame.getImageWidth()) || frameInfo->m_nHeight != static_cast<int>(frame.getImageHeight())) {
        LOGE(TAG, "frame size is not match the buffer size!");
        return false;
    }

    frameInfo->m_nWidth = frame.getImageWidth();
    frameInfo->m_nHeight = frame.getImageHeight();
    frameInfo->m_nBufferSize = frame.getImageSize();
    frameInfo->m_nPaddingX = frame.getImagePadddingX();
    frameInfo->m_nPaddingY = frame.getImagePadddingY();
    frameInfo->m_PixelType = frame.getImagePixelFormat();
    //frameInfo->m_pImageBuf = (BYTE *)malloc(sizeof(BYTE)* frameInfo->m_nBufferSize);

    memcpy(frameInfo->m_pImageBuf, frame.getImage(), frame.getImageSize());
#if 0 //save the image
    if (frameInfo->m_PixelType == Dahua::GenICam::gvspPixelMono8)
    {
        LOGD(TAG, "save the image!");
        QImage image = QImage(frameInfo->m_pImageBuf, frameInfo->m_nWidth,
                              frameInfo->m_nHeight, QImage::Format_Grayscale8);
        image.save("./frame.jpg");
    }
#endif

    LOGD(TAG, "got one frame from stream source end");
    return true;
}

bool PoeCamera::getMonoCameraImage(int id, CFrameInfo *frameInfo)
{
    return getframe(m_pMonoStreamSource[id], frameInfo);
}

CFrameInfo *PoeCamera::getCFrameInfo()
{
    return frameInfo;
}

void PoeCamera::onGetFrame(const CFrame &pFrame)
{
    //判断帧的有效性
    LOGD(TAG, "###get one frame!");
    bool isValid = pFrame.valid();
    if (!isValid) {
        LOGD(TAG, "frame is invalid!");
        return;
    } else {
        uint64_t blockId = pFrame.getBlockId();
        LOGD(TAG, "frame Id: %1", blockId);

        if (frameInfo->m_nWidth != static_cast<int>(pFrame.getImageWidth()) || frameInfo->m_nHeight != static_cast<int>(pFrame.getImageHeight())) {
            LOGE(TAG, "frame size is not match the buffer size!");
            return;
        }
        //isGrabbingFlag = false;
        frameInfo->m_nWidth = pFrame.getImageWidth();
        frameInfo->m_nHeight = pFrame.getImageHeight();
        frameInfo->m_nBufferSize = pFrame.getImageSize();
        frameInfo->m_nPaddingX = pFrame.getImagePadddingX();
        frameInfo->m_nPaddingY = pFrame.getImagePadddingY();
        frameInfo->m_PixelType = pFrame.getImagePixelFormat();
        //frameInfo->m_pImageBuf = (BYTE *)malloc(sizeof(BYTE) * frameInfo->m_nBufferSize);
        if(mCurImageBuf == 0) {
            frameInfo->m_pImageBuf = mBufferPong;
            mCurImageBuf = 1;
        }
        else
        {
            frameInfo->m_pImageBuf = mBufferPing;
            mCurImageBuf = 0;
        }
        LOGD(TAG, "frame width: %1", pFrame.getImageWidth());
        memcpy(frameInfo->m_pImageBuf, pFrame.getImage(), pFrame.getImageSize());
#if 0 //save the image
        if (frameInfo->m_PixelType == Dahua::GenICam::gvspPixelMono8)
        {
            LOGD(TAG, "save the image!");
            QImage image = QImage(frameInfo->m_pImageBuf, frameInfo->m_nWidth,
                                  frameInfo->m_nHeight, QImage::Format_Grayscale8);
            image.save("./config_file/pic/frame.bmp");
        }
#endif
        isGrabbingFlag = true;
        mListener->onFrame(mCameraId,mStationId,frameInfo);
    }

        return;
}

// need to free frameInfo.m_pImageBuf
void PoeCamera::SaveImage(int cameraid, CFrameInfo *frameInfo, QString path)
{
    if (frameInfo->m_PixelType == Dahua::GenICam::gvspPixelMono8)
    {
        QImage image = QImage(frameInfo->m_pImageBuf, frameInfo->m_nWidth,
                              frameInfo->m_nHeight, QImage::Format_Grayscale8);
        image.save(path);
    }
    return;
}

bool PoeCamera::CameraOpen()
{
    int i = mCameraId - mStationId;
    pCamera = m_vMonoCameraPtrList[i];
    if (NULL == pCamera)
    {
        LOGD(TAG, "connect camera fail. No camera.");
        return false;
    }

    if (true == pCamera->isConnected())
    {
        LOGD(TAG, "camera is already connected.");
        return false;
    }

    if (false == pCamera->connect())
    {
        LOGD(TAG, "connect camera fail.");
        return false;
    }

    LOGD(TAG, "connect camera ok.");
    return true;
}

bool PoeCamera::CameraClose()
{
    if (NULL == pCamera)
    {
        LOGD(TAG, "disconnect camera fail. No camera.");
        return false;
    }

    if (false == pCamera->isConnected())
    {
        LOGD(TAG, "camera is already disconnected.");
        return false;
    }

    if (false == pCamera->disConnect())
    {
        LOGD(TAG, "disconnect camera fail.");
        return false;
    }

    return true;
}

//启动相机
bool PoeCamera::CameraStart(bool continuemode, BYTE *bufPing, BYTE *bufPong, int width, int height)
{
    bool result = true;

    mBufferPing = bufPing;
    mBufferPong = bufPong;
    frameInfo->m_nWidth = width;
    frameInfo->m_nHeight = height;

    LOGD(TAG, "camera start. camera ID: %1", mCameraId);
    //设置外部触发配置
    result = setGrabMode(continuemode);
    if (result == false) {
        LOGD(TAG, "set grab mode fail!");
        return result;
    }

    //设置曝光时间
    result = setExposeTime(10000);
    if (result == false) {
        LOGD(TAG, "set ExposeTime fail!");
        return result;
    }

    //创建流对象
    m_pStreamSource = CSystem::getInstance().createStreamSource(pCamera);
    if (NULL == m_pStreamSource)
    {
        LOGD(TAG, "creat stream source fail!");
        result = false;

        return result;
    }
    m_pMonoStreamSource.push_back(m_pStreamSource);

    //注册回调函数
    if (!continuemode) {
        result = m_pStreamSource->attachGrabbing(IStreamSource::Proc(&PoeCamera::onGetFrame, this));
        if (result == false) {
            LOGD(TAG, "attach Grabbing fail!");
            CameraStop();

            return result;
        }
    }

    //开始拉流
    result = m_pStreamSource->startGrabbing(0, IStreamSource::EGrabStrategy::grabStrartegyLatestImage);
    if (result == false) {
        LOGD(TAG, "start Grabbing fail!");
        m_pStreamSource.reset();

        return result;
    }

    LOGD(TAG, "camera start ok!");

    return true;
}

//need do this when use trigger mode
bool PoeCamera::CameraRegGetFrameCallback()
{
    bool bRet = true;

    //注册回调函数
    bRet = m_pStreamSource->attachGrabbing(IStreamSource::Proc(&PoeCamera::onGetFrame, this));
    if(!bRet)
    {
        LOGD(TAG, "attch Grabbing fail.");
        //实际应用中应及时释放相关资源，如diconnect相机等，不宜直接return
        return false;
    }

    return true;
}

//need do this when use trigger mode
bool PoeCamera::CameraUnRegGetFrameCallback()
{
    bool bRet = true;

    //注册回调函数
    bRet = m_pStreamSource->detachGrabbing(IStreamSource::Proc(&PoeCamera::onGetFrame, this));
    if(!bRet)
    {
        LOGD(TAG, "attch Grabbing fail.");
        //实际应用中应及时释放相关资源，如diconnect相机等，不宜直接return
        return false;
    }

    return true;
}

bool PoeCamera::CameraStartGrabbing()
{
    bool result = true;

    //开始拉流
    if (!m_pStreamSource->startGrabbing(0, IStreamSource::EGrabStrategy::grabStrartegyLatestImage))
    {
        m_pStreamSource.reset();
        result = false;
        isGrabbingFlag = false;
    }
    else
    {
        isGrabbingFlag = true;
    }

    return result;
}

//停止相机拉流
bool PoeCamera::CameraStop()
{
    bool result = true;

    //unregister callback
    result = m_pStreamSource->detachGrabbing(IStreamSource::Proc(&PoeCamera::onGetFrame, this));
    if(false == result)
    {
        LOGD(TAG, "detach Grabbing fail.");
        //实际应用中应及时释放相关资源，如diconnect相机等，不宜直接return
    }

    result = m_pStreamSource->stopGrabbing();
    m_pStreamSource.reset();

    return result;
}

bool PoeCamera::setGrabMode(bool bContious)
{
    bool bRet = true;

    if (true == bContious)
    {
        //设置触发模式
        //CEnumNode nodeTrigerMode = sptrAcquisitionControl->triggerMode();
        CEnumNode nodeTrigerMode(pCamera, "TriggerMode");
        bRet = nodeTrigerMode.setValueBySymbol("Off");
        if (false == bRet)
        {
            LOGD(TAG, "set triggerMode fail.");
            return false;
        }
    }
    else
    {
        LOGD(TAG, "set Line trigger mode start!");
        /* 设置触发源为硬触发（软触发为Software） */
        //CEnumNode nodeTrigerSource = sptrAcquisitionControl->triggerSource();
        CEnumNode nodeTrigerSource(pCamera, "TriggerSource");
        bRet = nodeTrigerSource.setValueBySymbol("Line1");
        if (false == bRet)
        {
            LOGD(TAG, "set triggerSource fail.");
            return false;
        }

        //获取TriggerSelector属性操作对象
        //CEnumNode nodeTrigerSelector = sptrAcquisitionControl->triggerSelector();
        CEnumNode nodeTrigerSelector(pCamera, "TriggerSelector");
        bRet = nodeTrigerSelector.setValueBySymbol("FrameStart");
        if (false == bRet)
        {
            LOGD(TAG, "set TriggerSelector fail.");
            return false;
        }

        //CEnumNode nodeTrigerMode = sptrAcquisitionControl->triggerMode();
        CEnumNode nodeTrigerMode(pCamera, "TriggerMode");
        bRet = nodeTrigerMode.setValueBySymbol("On");
        if (false == bRet)
        {
           LOGD(TAG, "set triggerMode fail.");
           return false;
        }

        // 设置外触发为上升沿（下降沿为FallingEdge）RisingEdge
        //CEnumNode nodeTrigerActivation = sptrAcquisitionControl->triggerActivation();
        CEnumNode nodeTrigerActivation(pCamera, "TriggerActivation");
        bRet = nodeTrigerActivation.setValueBySymbol("RisingEdge");
        if (false == bRet)
        {
            LOGD(TAG, "set trigger activation fail.");
            return false;
        }

        //设置延迟时间
        bRet = setTriggerDelay(10);
        if (bRet == false) {
            LOGD(TAG, "set trigger delay fail!");
            return false;
        }

        LOGD(TAG, "set trigger mode end!");
    }

    return true;
}

bool PoeCamera::setExposeTime(double exposureTime)
{
    if (NULL == pCamera)
    {
        printf("Set ExposureTime fail. No camera or camera is not connected.\n");
        return false;
    }

    CDoubleNode nodeExposureTime(pCamera, "ExposureTime");

    if (false == nodeExposureTime.isValid())
    {
        printf("get ExposureTime node fail.\n");
        return false;
    }

    if (false == nodeExposureTime.isAvailable())
    {
        printf("ExposureTime is not available.\n");
        return false;
    }

    if (false == nodeExposureTime.setValue(exposureTime))
    {
        printf("set ExposureTime value = %f fail.\n", exposureTime);
        return false;
    }

    return true;
}

/* 设置外触发延时时间 */
bool PoeCamera::setTriggerDelay(double dDelayTime)
{
    bool bRet;

    CDoubleNode doubleNode(pCamera, "TriggerDelay");
    bRet = doubleNode.setValue(dDelayTime);
    if (false == bRet)
    {
        LOGD(TAG, "set triggerDelay fail.");
        return false;
    }

    return true;
}

bool PoeCamera::CameraEnumate(QString camera_serial)
{
    CSystem &systemObj = CSystem::getInstance();
    LOGD(TAG, "CameraEnumate start!!!!");
    bool bRet = systemObj.discovery(m_vCameraPtrList);
    if (false == bRet)
    {
        LOGD(TAG, "discovery device fail.");
        return false;
    }

    if (m_vCameraPtrList.size() < 1)
    {
        LOGD(TAG, "no devices.");
        return false;
    }

    QByteArray tmp_serial = camera_serial.toLatin1();
    char *MonoSerial = tmp_serial.data();

    for(int j = 0; j < m_vCameraPtrList.size(); j++) {
        ICameraPtr cameraSptr = m_vCameraPtrList[j];
        // 打印相机基本信息（key, 制造商信息, 型号, 序列号）
        LOGD(TAG, "Camera[%d] Info :%1", j);
        LOGD(TAG, "    key           = %1", cameraSptr->getKey());
        LOGD(TAG, "    vendor name   = %1", cameraSptr->getVendorName());
        LOGD(TAG, "    model         = %1", cameraSptr->getModelName());
        LOGD(TAG, "    serial number = %1", cameraSptr->getSerialNumber());
        if (strcmp(cameraSptr->getSerialNumber(), MonoSerial) == 0) {
            LOGD(TAG, "station device add");
            m_vMonoCameraPtrList.push_back(cameraSptr);
            break;
        }
        if(j >= m_vCameraPtrList.size())
        {
            LOGD(TAG, "the config file of camera list is not match!!");
            return false;
        }
    }

    return true;
}

void PoeCamera::setListener(CameraListener *listener)
{
    mListener = listener;
}

bool PoeCamera::CameraInit(void)
{
    LOGD(TAG, "CameraInit start");
    bool result = true;
#if 0
    bool ret;
    ret = CameraEnumate();
    if (ret == false)
        result = false;

    ret = CameraOpen();
    if (ret == false)
        result = false;

    ret = CameraStart();
    if (ret == false)
        result = false;

    setGrabModeAll();
#endif
    if (result == false)
        LOGD(TAG, "CameraInit failed");

    return result;
}

bool PoeCamera::CameraDeinit(void)
{
    bool result = true;
    bool ret;

    ret = CameraStop();
    if (ret == false) {
        LOGD(TAG, "Camera Stop failed!");
        CameraClose();
        return ret;
    }

    ret = CameraClose();
    if (ret == false) {
        LOGD(TAG, "Camera Close failed!");
        return ret;
    }

    LOGD(TAG, "Camera deinit done!");

    return result;
}
