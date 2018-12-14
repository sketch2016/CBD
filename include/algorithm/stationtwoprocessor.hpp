#ifndef STATIONTWOPROCESSOR_HPP
#define STATIONTWOPROCESSOR_HPP

#include "include/algorithm/baseprocessor.hpp"

struct statTwoRes
{
    bool stationOkFlag;
    QVector<int>errCode;
};

class StatusTwoprocess:public BaseProcess
{
public:
    StatusTwoprocess(ALGOINFO algorithm_info):mAlgoInfo(algorithm_info),frameIndex(0) {}
    ~StatusTwoprocess() {}
    bool getOpenStatus(Mat matImg);
    bool getLapStatus(Mat matImg);
    bool getHgzStatus(Mat matImg);
    bool getSdStatus(Mat matImg);
    statTwoRes doCheckStatusTwo(Mat matImg);

private:
    ALGOINFO mAlgoInfo;
    int frameIndex;
    Mat getInRectROI(Mat srcMatImg);
    Mat getWhiteBinImg(Mat srcMatImg);

    Mat getHgzMorImg(Mat srcMorImg);
    Mat getWhiteMorImg(Mat srcMorImg);

    Mat getSdRoiImg(Mat srcMorImg);
    Mat getSdBinImg(Mat srcImg);
    Mat getSdMorImg(Mat srcImg);
    Mat getAccInRectROI(Mat rectRoiImg);
    int getSdWidth(Mat sdRoiImg);

    bool checkWhiteRegionCount(Mat binMorImg);
    bool checkHgzRedBlackImg(Mat rectAccRoiImg, Mat hgzBinMorImg);
    bool CalHgzRedPix(Mat hgzRoiImg, Mat hgzMaskImg, int findhgzWidth, int findhgzHeight);
    bool CalHgzBlackPix(Mat hgzRoiImg, Mat hgzMaskImg, int findhgzWidth, int findhgzHeight);
};
#endif // STATIONTWOPROCESSOR_HPP
