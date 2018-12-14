#include "include/algorithm/algorithmwrapper.hpp"
#include "include/util/logger.h"

#define TAG "AlgorithmWrapper"

AlgorithmWrapper *AlgorithmWrapper::instance = nullptr;

AlgorithmWrapper *AlgorithmWrapper::getInstance() {
    if(instance == nullptr) {
        instance = new AlgorithmWrapper();
    }

    return instance;
}

bool AlgorithmWrapper::init(int stationId, int cameraNum, ALGOINFO algorithm_info)
{
    bool ret = true;

    LOGD(TAG, "AlgorithmWrapper init!");
    mCameraNum = cameraNum;
    for(int i = 0; i < cameraNum; i++) {
        ImageProcessor *mImageProcessor = new ImageProcessor();
        mImageProcessor->setListener(this);
        mImageProcessor->start(stationId, algorithm_info);

        mImageProcessorList.push_back(mImageProcessor);
    }

    return ret;
}

bool AlgorithmWrapper::deInit()
{
    for(int i = 0; i < mCameraNum; i++) {
        delete mImageProcessorList.at(i);
    }

    return true;
}

bool AlgorithmWrapper::start()
{
    //expansion interface
    return true;
}

bool AlgorithmWrapper::stop()
{
    //expansion interface
    return true;
}

bool AlgorithmWrapper::setFrame(FRAMEINFO &frame)
{
    LOGD(TAG, " get frame info, camera id: %1, station id: %2, timestamp: %3", frame.camera_id,frame.station_id,frame.timestamp);
    int curProcessorId = frame.camera_id - frame.station_id;
    mImageProcessorList.at(curProcessorId)->setFrame(frame.camera_id, frame.station_id, frame.data, frame.width, frame.height, frame.timestamp);

    return true;
}

void AlgorithmWrapper::postResultFromProcessor(AnalysisResult &totalResult)
{
    mAlgListener_->postResultFromAlgWrapper(totalResult);
}

void AlgorithmWrapper::setAlgListener(AlgorithmListener *listener)
{
    mAlgListener_ = listener;
}
