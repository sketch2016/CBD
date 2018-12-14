#include "include/controller/controller.h"
#include "include/util/logger.h"

#define TAG "ControllerService"

ControllerService *ControllerService::instance = nullptr;

CameraThread::CameraThread(ControllerService *instance, int id, int num):
    parent(instance),
    mStationId(id),
    mCameraNum(num),
    isStop(false),
    mStatus(CAMERA_THREAD_IDLE),
    mCurStation(STATION_THREE_ONE)
{

}

void CameraThread::takePicture() {
    if(mStatus == CAMERA_THREAD_IDLE) {
        mWaitMutex.lock();
        mWaitCond.wakeAll();
        mWaitMutex.unlock();
    }
}

void CameraThread::run() {
    while(!isStop) {
        mWaitMutex.lock();
        mWaitCond.wait(&mWaitMutex);
        mStatus = CAMERA_THREAD_RUNNING;
        mWaitMutex.unlock();

        LOGD(TAG, "start capture!");
        if(mStationId < STATION_THREE) {
            //CFrameInfo *frame = new CFrameInfo();
            LOGD(TAG, "station id: %1! buf id: %2", mStationId, mCurImagebuf[0]);
            QMap<int, CFrameInfo *> framelist;
            parent->mIlluControllerlist[0]->SetLed1(100);
            parent->mIlluControllerlist[0]->flush();

            QThread::msleep(200);
            frame[0] = parent->getCamera(0)->getCFrameInfo();
            if(mCurImagebuf[0] == IMAGE_BUFFER_PING)
            {
                frame[0]->m_pImageBuf = parent->getImageBuf(0, IMAGE_BUFFER_PING);
                mCurImagebuf[0] = IMAGE_BUFFER_PONG;
            }
            else if(mCurImagebuf[0] == IMAGE_BUFFER_PONG)
            {
                frame[0]->m_pImageBuf = parent->getImageBuf(0, IMAGE_BUFFER_PONG);
                mCurImagebuf[0] = IMAGE_BUFFER_PING;
            }
            parent->getCamera(0)->getMonoCameraImage(mStationId - 1, frame[0]);
            framelist.insert(mStationId+0, frame[0]);
            if(parent->mEventListener != nullptr) {
                parent->mEventListener->onFramesFromControl(mStationId, framelist);
            }

            //TODO
            //set this frame to algorithm
            struct timeval current_time;
            gettimeofday(&current_time, NULL);

            FRAMEINFO frameInfo;
            frameInfo.camera_id = mStationId + 0;
            frameInfo.station_id = mStationId;
            frameInfo.data = (uchar *)frame[0]->m_pImageBuf;
            //frameInfo.format = frame->m_PixelType;
            frameInfo.height = frame[0]->m_nHeight;
            frameInfo.width = frame[0]->m_nWidth;
            frameInfo.timestamp = current_time.tv_sec*1000 + current_time.tv_usec/1000;
            frameInfo.force_detect = true;

            parent->mAlgWrapper->setFrame(frameInfo);

            mStatus = CAMERA_THREAD_IDLE;
        }
        else if(mStationId == STATION_THREE) {
            if(mCurStation == STATION_THREE_ONE)
            {
                QMap<int, CFrameInfo *> framelist;
                parent->mIlluControllerlist[0]->SetLed1(100);
                parent->mIlluControllerlist[0]->SetLed2(100);
                parent->mIlluControllerlist[0]->SetLed3(0);
                parent->mIlluControllerlist[0]->SetLed4(0);
                parent->mIlluControllerlist[0]->flush();

                QThread::msleep(200);
                for(int i = 0; i < 2; i++) {
                    frame[i] = parent->getCamera(i)->getCFrameInfo();
                    if(mCurImagebuf[i] == IMAGE_BUFFER_PING)
                    {
                        frame[i]->m_pImageBuf = parent->getImageBuf(i, IMAGE_BUFFER_PING);
                        mCurImagebuf[i] = IMAGE_BUFFER_PONG;
                    }
                    else if(mCurImagebuf[i] == IMAGE_BUFFER_PONG)
                    {
                        frame[i]->m_pImageBuf = parent->getImageBuf(i, IMAGE_BUFFER_PONG);
                        mCurImagebuf[i] = IMAGE_BUFFER_PING;
                    }

                    parent->getCamera(i)->getMonoCameraImage(i, frame[i]);
                    struct timeval current_time_one;
                    gettimeofday(&current_time_one, NULL);

                    FRAMEINFO frameInfo;
                    frameInfo.camera_id = mStationId + i;
                    frameInfo.station_id = mStationId;
                    frameInfo.data = (uchar *)frame[i]->m_pImageBuf;
                    //frameInfo.format = frame->m_PixelType;
                    frameInfo.height = frame[i]->m_nHeight;
                    frameInfo.width = frame[i]->m_nWidth;
                    frameInfo.timestamp = current_time_one.tv_sec*1000 + current_time_one.tv_usec/1000;
                    frameInfo.force_detect = true;

                    parent->mAlgWrapper->setFrame(frameInfo);
                    framelist.insert(mStationId+i, frame[i]);
                }
                if(parent->mEventListener != nullptr) {
                    parent->mEventListener->onFramesFromControl(mStationId, framelist);
                }

                mStatus = CAMERA_THREAD_IDLE;
                mCurStation = STATION_THREE_TWO;
            }
            else if(mCurStation == STATION_THREE_TWO)
            {
                QMap<int, CFrameInfo *> framelist;
                parent->mIlluControllerlist[0]->SetLed1(0);
                parent->mIlluControllerlist[0]->SetLed2(0);
                parent->mIlluControllerlist[0]->SetLed3(100);
                parent->mIlluControllerlist[0]->SetLed4(100);
                parent->mIlluControllerlist[0]->flush();

                QThread::msleep(200);
                for(int i = 2; i < 4; i++) {
                    frame[i] = parent->getCamera(i)->getCFrameInfo();
                    if(mCurImagebuf[i] == IMAGE_BUFFER_PING)
                    {
                        frame[i]->m_pImageBuf = parent->getImageBuf(i, IMAGE_BUFFER_PING);
                        mCurImagebuf[i] = IMAGE_BUFFER_PONG;
                    }
                    else if(mCurImagebuf[i] == IMAGE_BUFFER_PONG)
                    {
                        frame[i]->m_pImageBuf = parent->getImageBuf(i, IMAGE_BUFFER_PONG);
                        mCurImagebuf[i] = IMAGE_BUFFER_PING;
                    }

                    parent->getCamera(i)->getMonoCameraImage(i, frame[i]);
                    struct timeval current_time;
                    gettimeofday(&current_time, NULL);

                    FRAMEINFO frameInfo;
                    frameInfo.camera_id = mStationId + i;
                    frameInfo.station_id = mStationId;
                    frameInfo.data = (uchar *)frame[i]->m_pImageBuf;
                    //frameInfo.format = frame->m_PixelType;
                    frameInfo.height = frame[i]->m_nHeight;
                    frameInfo.width = frame[i]->m_nWidth;
                    frameInfo.timestamp = current_time.tv_sec*1000 + current_time.tv_usec/1000;
                    frameInfo.force_detect = true;

                    parent->mAlgWrapper->setFrame(frameInfo);
                    framelist.insert(mStationId+i, frame[i]);
                }

                if(parent->mEventListener != nullptr) {
                    parent->mEventListener->onFramesFromControl(mStationId, framelist);
                }
                mStatus = CAMERA_THREAD_IDLE;
                mCurStation = STATION_THREE_ONE;
            }
        }
    }
}

ICamera *ControllerService::getCamera(int id) {
    return mCameraList[id];
}

ControllerService *ControllerService::getInstance() {
    if(instance == nullptr) {
        instance = new ControllerService();
    }

    return instance;
}

ControllerService::ControllerService() {

}

ControllerService::~ControllerService() {
    //free the image buffer
    for(int i = 0; i < mImageBufPing.size(); i++) {
        free(mImageBufPing.at(i));
    }
    for(int j = 0; j < mImageBufPong.size(); j++) {
        free(mImageBufPong.at(j));
    }
}

bool ControllerService::initAll(STATIONINFO station_info, ALGOINFO algorithm_info) {
    //相机初始化,需要先根据配置文件确认当前相机个数以及跟工位对应关系
    bool ret = true;
    int station_id = station_info.station_id;
    QMap<int,QString> camera_list = station_info.camera_serial_num;
    int cameraNum = camera_list.count();
    int width = station_info.image_width;
    int height = station_info.image_height;
    bool continue_mode = true;
    mContrStatus = false;

    LOGD(TAG, "station id is: %1, camera number: %2", station_id, cameraNum);
    for (int i = 0; i < cameraNum; i++) {
        //alloc the image buffer
        BYTE *ImageBufPingTmp = (BYTE *)malloc(sizeof(BYTE) * (width*height));
        mImageBufPing.push_back(ImageBufPingTmp);
        BYTE *ImageBufPongTmp = (BYTE *)malloc(sizeof(BYTE) * (width*height));
        mImageBufPong.push_back(ImageBufPongTmp);

        //creat camera object
        ICamera *camera = new PoeCamera();
        ret = camera->CameraEnumate(camera_list.value(i));
        if(!ret) {
            LOGD(TAG, "the camera config of station is error!");
            return false;
        }
        camera->setCameraId(station_id + i);
        camera->setStationId(station_id);
        camera->setListener(this);
        ret = camera->CameraOpen();
        if(!ret) {
            camera->CameraClose();
            return ret;
        }
        ret = camera->CameraStart(continue_mode, ImageBufPingTmp, ImageBufPingTmp, width, height);
        if(!ret) {
            camera->CameraClose();
            return ret;
        }

        mCameraList.append(camera);
    }
    //如果不是trigger mode, 需要起一个thread来不停的抓帧
    if (continue_mode) {
        mCameraThread = new CameraThread(this, station_id, cameraNum);
        mCameraThread->start();
    }
    LOGD(TAG, "controllerService camera init end!");

    //光源初始化，超过4个camera时，需要两个光源控制器
    //int Optoelec_num = station_info.optoelectronic_num;
    int illumControl_num = station_info.illumController_num;
    QMap<int,QString> serial_list = station_info.serial_port;
    for(int j = 0; j < illumControl_num; j++) {
        IlluminantController *IlluControl = new IlluminantController();
        //两个光源控制器需要两个串口设备
        char *dev_name = serial_list.value(j).toLatin1().data();
        LOGD(TAG, "dev name: %1", dev_name);
        //char *dev_name = "/dev/ttyS4";
        IlluControl->init(dev_name);
        //设置灯源为trigger模式触发,并且设置灯的亮度值，trigger触发后就按照此亮度值亮
        IlluControl->setTriggerMode(true);
        IlluControl->setBrightness(100,100,50,30);
        mIlluControllerlist.append(IlluControl);
    }

    //init algorithm
    mAlgWrapper = AlgorithmWrapper::getInstance();
    mAlgWrapper->setAlgListener(this);
    mAlgWrapper->init(station_id, cameraNum, algorithm_info);

    mContrStatus = true;

    return true;
}

void ControllerService::onFrame(int cameraid, int stationid, CFrameInfo *frame) {
    //got one frame in controller, and set it to algorithm
    struct timeval current_time;
    gettimeofday(&current_time, NULL);

    FRAMEINFO frameInfo;
    frameInfo.camera_id = cameraid;
    frameInfo.station_id = stationid;
    frameInfo.data = (uchar *)frame->m_pImageBuf;
    //frameInfo.format = frame->m_PixelType;
    frameInfo.height = frame->m_nHeight;
    frameInfo.width = frame->m_nWidth;
    frameInfo.timestamp = current_time.tv_sec*1000 + current_time.tv_usec/1000;
    frameInfo.force_detect = true;

    //set this frame to algorithm
    mAlgWrapper->setFrame(frameInfo);
    //send frame to controller
    LOGD(TAG, "send one frame to controller!");
    QMap<int, CFrameInfo *> framelist;
    framelist.insert(cameraid, frame);
    mEventListener->onFramesFromControl(stationid, framelist);
    //TODO:
    //when the trigger is continue on, the frame will be sent continued.
}

void ControllerService::postResultFromAlgWrapper(AnalysisResult &result) {
    //callback form algorithm wrapper
    mEventListener->onAnalysisResult(result);
}

void ControllerService::savePic(int cameraid, CFrameInfo *frm, QString path) {

    mCameraList[cameraid]->SaveImage(cameraid, frm, path);
}

void ControllerService::addEventListener(EventListener *listener) {
    this->mEventListener = listener;
}

void ControllerService::takePicture() {
    if(mContrStatus == true)
        mCameraThread->takePicture();
    else
        LOGE(TAG, "ControllerService intAll has failed!");
}

BYTE *ControllerService::getImageBuf(int id, int ping_pong) {
    if(ping_pong == IMAGE_BUFFER_PING)
        return mImageBufPong[id];
    else
        return mImageBufPing[id];
}


