#ifndef ALGORITHMWRAPPER_HPP
#define ALGORITHMWRAPPER_HPP

#include "include/algorithm/imageprocessor.hpp"

enum COLOR_FMT {
    COLOR_FMT_OPENCV_BGR = 0,
    COLOR_FMT_OPENCV_RGB,
    COLOR_FMT_MAX
};

typedef struct FrameInformation {
    int camera_id;
    int station_id;
    int width;
    int height;
    int timestamp;
    COLOR_FMT format;
    bool    force_detect;
    uchar    *data;
} FRAMEINFO;

/**
 * @brief Object detection listener, user(s) must set this entry with init(...).
 */
class AlgorithmListener
{
public:
    //virtual void postEvent(int event_type, int ext1, int ext2, void *value) = 0;

    virtual void postResultFromAlgWrapper(AnalysisResult &totalResult) = 0;
};

class AlgorithmWrapper:public ProcessorListener
{
public:
    AlgorithmWrapper() {}
    ~AlgorithmWrapper() {}
    static AlgorithmWrapper *instance;
    static AlgorithmWrapper *getInstance();

    bool init(int stationId, int cameraNum, ALGOINFO algorithm_info);
    bool deInit();
    void setAlgListener(AlgorithmListener *listener);
    bool start();
    bool stop();
    bool setFrame(FRAMEINFO &frame);
    void postResultFromProcessor(AnalysisResult &totalResult);

private:
    AlgorithmListener *mAlgListener_;
    QVector<ImageProcessor *> mImageProcessorList;
    int mCameraNum;
};

#endif // ALGORITHMWRAPPER_HPP
