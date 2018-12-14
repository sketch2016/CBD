#include <opencv/cv.h>

#include "include/util/imageutil.h"
#include "include/util/logger.h"
#include "include/hardware/cframeinfo.h"

#define TAG "Image Util"

ImageUtil::ImageUtil()
{

}

QImage ImageUtil::MatToQImage(const cv::Mat &mat){
    // 8-bits unsigned, NO. OF CHANNELS=1
    if(mat.type()==CV_8UC1) {
        // Set the color table (used to translate colour indexes to qRgb values)
        QVector<QRgb> colorTable;
        for (int i=0; i<256; i++) {
            colorTable.push_back(qRgb(i,i,i));
        }

        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);
        img.setColorTable(colorTable);
        return img;
    }
    // 8-bits unsigned, NO. OF CHANNELS=3
    if(mat.type()==CV_8UC3) {
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return img.rgbSwapped();
    } else {
        LOGE(TAG,"QImage gen failed!!!");
        return QImage();
    }
}

void ImageUtil::MatArrayToQImages(const QVector<cv::Mat> &mats, QVector<QImage> &images)
{
    for (const cv::Mat &mat : mats) {
        images.append(MatToQImage(mat));
    }
}

void ImageUtil::FramesToImages(const QVector<CFrameInfo> &frames, QVector<QImage*> &images) {

    for (CFrameInfo frameInfo : frames) {
        QImage *image = new QImage(frameInfo.m_pRGBbuffer, frameInfo.m_nWidth, frameInfo.m_nHeight, QImage::Format_RGB888);
        images.append(image);
    }

}
