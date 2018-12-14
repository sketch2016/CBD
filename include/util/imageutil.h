#ifndef IMAGEUTIL_H
#define IMAGEUTIL_H

#include <opencv/cv.h>
#include <QImage>
#include "include/hardware/cframeinfo.h"

class ImageUtil
{
public:
    ImageUtil();
    static QImage MatToQImage(const cv::Mat& mat);
    static void MatArrayToQImages(const QVector<cv::Mat>& mats, QVector<QImage> &images);
    void FramesToImages(const QVector<CFrameInfo>& frames, QVector<QImage*> &images);
};

#endif // IMAGEUTIL_H
