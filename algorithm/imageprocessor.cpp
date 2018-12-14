#include "include/algorithm/imageprocessor.hpp"
#include "include/util/logger.h"

#define STATONE 1
#define STATTWO 2
#define TAG "ImageProcessor"

//int indexNum = 0;
//char srcImgFileOne[100] = "/home/xiaoqi/work/tobaccoBox/qt/snap/status1/testSrc/";
//char srcImgFileTwo[100] = "/home/xiaoqi/work/tobaccoBox/qt/snap/status2/srcTest/";

bool ImageProcessor::start(int stationId, ALGOINFO algorithm_info)
{
    bool ret = true;
    if(mImageProThread == nullptr) {
        mImageProThread = new ImageProcessThread(this);
        mImageProThread->start();
    }
    else {
        LOGE(TAG, "Image process thread creat failed!");
        ret = false;
    }

    mVideoFrame = new VIDEOFRAME();
    //mAlgoInfo = &algorithm_info;
    if (stationId == 1)
        mStationOneProcessor = new StatOnePross(algorithm_info);
    else if(stationId == 2)
        mStationTwoProcessor = new StatusTwoprocess(algorithm_info);

    return ret;
}

bool ImageProcessor::stop()
{
    mImageProThread->mStatus = false;

    delete mImageProThread;
    mImageProThread = nullptr;
    delete mStationOneProcessor;
    mStationOneProcessor = nullptr;

    return true;
}

bool ImageProcessor::setFrame(int camera_id, int station_id, uchar *data, int width, int height, int timestamp)
{
    //VIDEOFRAME frame_info;
    //notify the image process thread
    mImageProThread->mutex_.lock();

    mVideoFrame->camera_id = camera_id;
    mVideoFrame->station_id = station_id;
    mVideoFrame->timestamp = timestamp;
    mVideoFrame->width = width;
    mVideoFrame->height = height;

    Mat matImg;
    if(station_id == STATONE)
    {
        QImage image = QImage((uchar *)data, width, height, QImage::Format_Grayscale8);
        matImg = cv::Mat(height, width, CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
    }
    else if(station_id == STATTWO)
    {
        QImage image = QImage((uchar *)data, width, height, QImage::Format_RGB888);
        matImg = cv::Mat(height, width, CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
    }
    else
    {
        QImage image = QImage((uchar *)data, width, height, QImage::Format_Grayscale8);
        matImg = cv::Mat(height, width, CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
    }

    mVideoFrame->frame = matImg.clone();

#if 0  // read the image to debug
    if(camera_id == 1)
    {
        char srcImgName[100];
        sprintf(srcImgName, "%s%d%s", srcImgFileOne, indexNum, ".bmp");
        indexNum = indexNum +1;
        Mat matFrame = cv::imread(srcImgName);
        mVideoFrame->frame = matFrame;
    }
    else if (camera_id == 2)
    {
        char srcImgName[100];
        sprintf(srcImgName, "%s%d%s", srcImgFileTwo, indexNum, ".jpg");
        indexNum = indexNum +1;
        Mat matFrame = cv::imread(srcImgName);
        mVideoFrame->frame = matFrame;
    }
    else
    {
        cout<<"debug the status three \n"<<endl;
    }
#endif
    LOGD(TAG, "got one frame and notify the image process thread!");

    mImageProThread->mWaitCond.wakeAll();
    mImageProThread->mutex_.unlock();

    return true;
}

void ImageProcessor::ImageProcessThread::run()
{
    LOGD(TAG, "image process thread start!");
    while(mStatus) {
        mutex_.lock();
        mWaitCond.wait(&mutex_);
        //status = CAMERA_THREAD_RUNNING;
        instance_->processFrame(instance_->mVideoFrame);
        mutex_.unlock();
    }
}

bool ImageProcessor::processFrame(VIDEOFRAME *frame)
{
    oneFrameResult mFrameResult;
    AnalysisResult mTotalResult;
    if(frame->station_id == 1) {
        algorithmOne(frame, mFrameResult);

        if(mFrameResult.frame_result == true)
        {
            mTotalResult.actual_result = 1;
            mTotalResult.results.push_back(mFrameResult);
        }
        else
        {
            mTotalResult.actual_result = 0;
            mTotalResult.results.push_back(mFrameResult);
        }
    }
    else if(frame->station_id == 2) {
        algorithmTwo(frame, mFrameResult);

        if(mFrameResult.frame_result == true)
        {
            mTotalResult.actual_result = 1;
            mTotalResult.results.push_back(mFrameResult);
        }
        else
        {
            mTotalResult.actual_result = 0;
            mTotalResult.results.push_back(mFrameResult);
        }
    }
    else if(frame->station_id == 3) {
        if(frame->camera_id == 3) {
            algorithmThree(frame, mFrameResult);
        }
        else if(frame->camera_id == 4) {
            algorithmThree(frame, mFrameResult);
        }
        dataSaveMutex.lock();
        mTotalResult.actual_result = 0;
        mTotalResult.results.push_back(mFrameResult);
        dataSaveMutex.unlock();
    }

    mListener->postResultFromProcessor(mTotalResult);

    return true;
}

void ImageProcessor::setListener(ProcessorListener *listener)
{
    mListener = listener;
}

bool ImageProcessor::algorithmOne(VIDEOFRAME *frm, oneFrameResult &mProcessResult)
{
    statOneRes stationOneResult = mStationOneProcessor->doCheckStatusOne(frm->frame);
    mProcessResult.camera_id = frm->camera_id;
    mProcessResult.frame_result = stationOneResult.stationOkFlag;

    if (mProcessResult.frame_result == true)
    {
        mProcessResult.error_codes.push_back(NoError);
    }
    else
    {
        for(int i=0; i<stationOneResult.errCode.count();i++)
        {
            int errRmp = stationOneResult.errCode.at(i);
            mProcessResult.error_codes.push_back(errRmp);
        }
    }

    defect_area defectArea;
    defectArea.height = 0;
    defectArea.width = 0;
    defectArea.p.x = 0;
    defectArea.p.y = 0;
    mProcessResult.defect_areas.push_back(defectArea);
    LOGD(TAG, "algorithmOne:image process one frame end!");

    return true;
}

bool ImageProcessor::algorithmTwo(VIDEOFRAME *frm, oneFrameResult &mProcessResult)
{
    statTwoRes stationTwoResult = mStationTwoProcessor->doCheckStatusTwo(frm->frame);
    mProcessResult.camera_id = frm->camera_id;
    mProcessResult.frame_result = stationTwoResult.stationOkFlag;
    if (mProcessResult.frame_result == true)
    {
        mProcessResult.error_codes.push_back(NoError);
    }
    else
    {
        for(int i=0; i<stationTwoResult.errCode.count();i++)
        {
            int errRmp = stationTwoResult.errCode.at(i);
            mProcessResult.error_codes.push_back(errRmp);
        }
    }

    defect_area defectArea;
    defectArea.height = 0;
    defectArea.width = 0;
    defectArea.p.x = 0;
    defectArea.p.y = 0;
    mProcessResult.defect_areas.push_back(defectArea);
    LOGD(TAG, "algorithmTwo: image process one frame end!");

    return true;
}

bool ImageProcessor::algorithmThree(VIDEOFRAME *frame, oneFrameResult &mProcessResult)
{
    //TODO:
    //image processing
    //
    mProcessResult.camera_id = frame->camera_id;
    mProcessResult.frame_result = 0;
    mProcessResult.error_codes.push_back(0);

    defect_area defectArea;
    defectArea.height = 0;
    defectArea.width = 0;
    defectArea.p.x = 0;
    defectArea.p.y = 0;
    mProcessResult.defect_areas.push_back(defectArea);
    LOGD(TAG, "image process end!");

    return true;
}

bool ImageProcessor::algorithmFour(VIDEOFRAME *frame, oneFrameResult &mProcessResult)
{
    //TODO:
    //image processing
    //
    mProcessResult.camera_id = frame->camera_id;
    mProcessResult.frame_result = 0;
    mProcessResult.error_codes.push_back(0);

    defect_area defectArea;
    defectArea.height = 0;
    defectArea.width = 0;
    defectArea.p.x = 0;
    defectArea.p.y = 0;
    mProcessResult.defect_areas.push_back(defectArea);
    LOGD(TAG, "image process end!");

    return true;
}
