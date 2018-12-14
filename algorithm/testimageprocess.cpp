#include "include/manager/servicemanager.h"
#include "include/util/logger.h"
#include "include/defines.h"
#include "include/algorithm/testimageprocess.h"

#define TAG "testimageprocess"

TestImageAloProcess::TestImageAloProcess(void)
{

}

Config_Params TestImageAloProcess::getConfigParams()
{
    Config_Params params;
    configService->readAlgoConfigFromFile(params);
    configService->readDistortParamFromFile(params);

    return params;
}

void TestImageAloProcess::sendFrameToAlgo(int camId, int statId)
{
    struct timeval current_time;
    gettimeofday(&current_time, NULL);

    FRAMEINFO frameInfo;
    frameInfo.camera_id = camId;
    frameInfo.station_id = statId;
    frameInfo.data = (uchar *)"aaaaabbbb";

    frameInfo.height = 2;
    frameInfo.width = 2;
    frameInfo.timestamp = current_time.tv_sec*1000 + current_time.tv_usec/1000;
    frameInfo.force_detect = true;
    mAlgWrapper->setFrame(frameInfo);
}

void TestImageAloProcess::startTest()
{
    int station_id = 1;
    int cameraNum = 1;
    configService = ConfigService::getInstance();
    Config_Params params = getConfigParams();
    mAlgWrapper = AlgorithmWrapper::getInstance();
    mAlgWrapper->init(station_id, cameraNum, params.algorithm_info);
    QThread::msleep(10);

    for(int i=0; i<2; i++)
    {
        LOGD(TAG, "going to send frame, index is:  %1", i);
        sendFrameToAlgo(cameraNum, station_id);
        QThread::msleep(500);
    }

    QThread::msleep(20000);
}




