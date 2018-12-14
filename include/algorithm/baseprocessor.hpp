#ifndef BASEPROCESSOR_HPP
#define BASEPROCESSOR_HPP

#include <QVector>
#include "include/defines.h"

using namespace std;
using namespace cv;

struct rectInfors
{
    int width;
    int heigh;
    bool rectOkFlag;
};

struct AreaCmp
{
    AreaCmp(const vector<float>& _areas):areas(&_areas) {}
    bool operator ()(int a, int b) const {return (*areas)[a] > (*areas)[b];}
    const vector<float> *areas;
};

class  BaseProcess
{
public:
    BaseProcess() {}
    ~BaseProcess() {}
    void swapValue(int &a, int &b);
    Mat getUndistPersp(Mat srcImg);
    Mat getBinImg(Mat matImg, int grayThresh);
    Mat getHSVBinImg(Mat matImg, Scalar hsvPara, Scalar HSVPara);
    Mat getBGRBinImg(Mat matImg, Scalar bgrPara, Scalar BGRPara);
    Mat getAdBinImg(Mat matImg, int blockSize, int constValue);
    Mat getUndistPersp(Mat matImg, Mat camMat, Mat distMat, Mat perspMat);
    Mat getScaleImg(Mat srcImg, int scaleParam);
    Mat getRoiImg(Mat srcImg, Rect roiRect);

    float blackCountRation(Mat openBinyImg, int openResizeThreshs);
    bool getOpenCheck(Mat srcMatImg, int openGrayThreshs, float openStatueRation);

private:
     Mat binImg;

};

#endif // BASEPROCESSOR_HPP
