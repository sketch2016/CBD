#include "include/manager/servicemanager.h"
#include "include/util/logger.h"
#include "include/defines.h"

#define TAG "ServiceManager"

ServiceManager::ServiceManager()
{
    configService = ConfigService::getInstance();
    mainWindow = new MainWindow();
    communicationService = new CommunicationService();
    storageService = StorageService::getInstance();
    mainWindow->setListener(this);
    mainWindow->addConfigChangeListener(configService);
    mainWindow->show();
    mControllerService = ControllerService::getInstance();
    mControllerService->addEventListener(this);

    //read config.ini
    Config_Params params = getConfigParams();
    station = params.station_info.station_id;
    communicationService->init(params.plcAddr, params.displayAddr, params.plcServerAddr);
    mainWindow->applyConfig(params);
    storageService->applyConfig(params);
    mControllerService->initAll(params.station_info, params.algorithm_info);

    Detect_Data detect_Data = {0,0,""};
    storageService->getDetectCount(detect_Data);
    mainWindow->initDetectData(detect_Data);

    communicationService->setConnectListener(mainWindow);
    communicationService->setTransporterStopListener(this);


}

ServiceManager::~ServiceManager()
{
    if (communicationService) {
        communicationService->stop();
    }

    if (configService) {
        delete configService;
    }

    if (mainWindow) {
        delete mainWindow;
    }

    if (communicationService) {
        delete communicationService;
    }

    if (storageService) {
        delete storageService;
    }

    if (mControllerService) {
        delete mControllerService;
    }
}

void ServiceManager::exec()
{
    communicationService->start();
}

void ServiceManager::cancel()
{
    communicationService->stop();
}

void ServiceManager::onAnalysisResult(AnalysisResult &result)
{
    LOGD(TAG, "onAnalysisResult");
    Detect_info detect_info;
    detect_info.machine_state = Machine_State::Running;
    detect_info.mStation = station;
    detect_info.current_detect_result = result.actual_result;
    for (oneFrameResult ofResult: result.results) {
        detect_info.errorCodes.append(ofResult.error_codes);
    }
    //detect_info.resultCodes = result->resultCodes;
    communicationService->setDetectInfo(detect_info);

//    QVector<QImage*> images;
//    imageUtil.FramesToImages(result->frames, images);
//    mainWindow->showImages(images);
//    storageService->saveImages(images, result->detect_Result, result->resultCodes);

//    for (QImage *image : images) {
//        delete image;
//    }

    Detect_Data add_data;
    add_data.total = 1;
    if (result.actual_result == Detect_Result::Unqualified) {
        add_data.unqualified = 1;
    }
    storageService->updateAndStore(add_data, &result);
    mainWindow->updateDetectData(add_data);
}

void ServiceManager::onFramesFromControl(int stationid, QMap<int, CFrameInfo *> framelist)
{
   LOGD(TAG, "got one frame from control. station id: %1", stationid);
   int cameraid = 0;
   CFrameInfo *frame;
   QMap<int, CFrameInfo *>::iterator iter = framelist.begin();
   for(; iter != framelist.end(); iter++) {
       cameraid = iter.key();
       frame = iter.value();
       LOGD(TAG, "########camera id: %1", cameraid);
       onFrameFromControl(stationid, cameraid, frame);
       //framelist.erase(iter);
   }
}

void ServiceManager::onFrameFromControl(int stationid, int cameraid, CFrameInfo *frame)
{
   LOGD(TAG, "onFrame callback, got one frame from control. camera id: %1", cameraid);

   QImage image;
   if(frame != nullptr ) {
       LOGD(TAG, "window onframe, width: %1, height: %2", frame->m_nWidth, frame->m_nHeight);
       image = QImage(frame->m_pImageBuf,frame->m_nWidth,frame->m_nHeight,QImage::Format_Grayscale8);

       QPixmap pixmap = QPixmap::fromImage(image);
       mainWindow->showImage(cameraid, pixmap);
       storageService->addImage(cameraid, image);
       //ui->icon->setPixmap(pixmap);
       //image.save("./config_file/pic/frame.jpg");
 #if 0
       oneFrameResult ofr = {1,1,{0},{}};
       AnalysisResult result;
       result.actual_result = 1;
       result.results = QVector<oneFrameResult *>();
       result.results.append(&ofr);

       Detect_Data add_data;
       add_data.total = 1;
       if (result.actual_result == Detect_Result::Unqualified) {
           add_data.unqualified = 1;
       }
       storageService->updateAndStore(add_data, &result);
#endif
   }
}

void ServiceManager::onFrames(QVector<CFrameInfo> &frames)
{
    LOGD(TAG, "onFrames:frames.size = %1", frames.length());
//    QVector<QImage> images = imageUtil.FramesToImages(frames);
//    mainWindow->showImages(images);
//    storageService->saveImages(images, result->detect_Result, result->resultCodes);

}

void ServiceManager::onTransporterStopped()
{
    LOGD(TAG, "onTransporterStopped, notify mControllerService to take picture");
    mControllerService->takePicture();
}

Config_Params ServiceManager::getConfigParams()
{
    Config_Params params;
    configService->readConfigFromFile(params);
    configService->readAlgoConfigFromFile(params);
    configService->readDistortParamFromFile(params);

    return params;
}

void ServiceManager::onStartClicked()
{
    LOGD(TAG, "onStartClicked");
    this->exec();
    onTransporterStopped();
}

void ServiceManager::onStopClicked()
{
    LOGD(TAG, "onStopClicked");
    this->cancel();
}

void ServiceManager::onDefectPicsClicked()
{
    LOGD(TAG, "onDefectPicsClicked");
}

void ServiceManager::onBrandSelectorIndexChange()
{
    LOGD(TAG, "onBrandSelectorIndexChange");
}
