#ifndef IMAGEPROCESSOR_HPP
#define IMAGEPROCESSOR_HPP

#include <QMutex>
#include <QWaitCondition>
#include <QThread>
#include <sys/time.h>
#include <QVector>

#include <opencv2/dnn.hpp>
#include <opencv2/dnn/shape_utils.hpp>
//#include <opencv2/imgproc.hpp>
//#include <opencv2/imgcodecs.hpp>
//#include <opencv2/highgui.hpp>

#include "include/defines.h"
#include "include/algorithm/stationoneprocessor.hpp"
#include "include/algorithm/stationtwoprocessor.hpp"

typedef struct VideoFrameInformation {
    int32_t camera_id;
    int32_t station_id;
    int32_t width;
    int32_t height;
    int64_t timestamp;
    bool    force_detect;
    cv::Mat frame;
} VIDEOFRAME;

class ProcessorListener
{
public:
    virtual void postResultFromProcessor(AnalysisResult &totalResult) = 0;

};

class ImageProcessor
{
public:
    ImageProcessor():mImageProThread(nullptr),mListener(nullptr) {}
    ~ImageProcessor() {}

    bool setFrame(int camera_id, int station_id, uchar *data, int width, int height, int timestamp);
    bool start(int stationId, ALGOINFO algorithm_info);
    bool stop();
    bool processFrame(VIDEOFRAME *frame);
    void setListener(ProcessorListener *listener);

    class ImageProcessThread:public QThread {
    public:
        ImageProcessThread(ImageProcessor *mInstance):
            instance_(mInstance) {}
        ~ImageProcessThread() {}

        QMutex mutex_;
        QWaitCondition mWaitCond;
        bool mStatus = true;
    private:
        void run();
        ImageProcessor *instance_;
    };

private:
    bool algorithmOne(VIDEOFRAME *frm, oneFrameResult &mProcessResult);
    bool algorithmTwo(VIDEOFRAME *frm, oneFrameResult &mProcessResult);
    bool algorithmThree(VIDEOFRAME *frm, oneFrameResult &mProcessResult);
    bool algorithmFour(VIDEOFRAME *frm, oneFrameResult &mProcessResult);

    ALGOINFO *mAlgoInfo;
    VIDEOFRAME *mVideoFrame;
    ImageProcessThread *mImageProThread;
    ProcessorListener *mListener;
    StatOnePross *mStationOneProcessor;
    StatusTwoprocess *mStationTwoProcessor;
    QMutex dataSaveMutex;
};
#endif // IMAGEPROCESSOR_HPP
