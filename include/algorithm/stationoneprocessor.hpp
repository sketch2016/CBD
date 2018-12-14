#ifndef STATIONONEPROCESSOR_HPP
#define STATIONONEPROCESSOR_HPP

#include "include/algorithm/baseprocessor.hpp"

#define OPENSTATUSCHECKOK     1
#define OPENSTATUSCHECKNOTOK  0
#define NOTOPENSTATUS         -1

struct statOneRes
{
    bool stationOkFlag;
    QVector<int>errCode;
};


struct radRectInfors
{
    bool radResFlag;
    RotatedRect radRotRect;
    Mat findRectBinImg;
};


class StatOnePross:public BaseProcess
{
public:
    StatOnePross(ALGOINFO algorithm_info):mAlgoInfo(algorithm_info),frameCount(0) {}
    ~StatOnePross() {}
    bool getOpenStatue(Mat matImg);
    rectInfors getRectInfors(Mat matImg);
    statOneRes doCheckStatusOne(Mat srcMatImg);

private:
    ALGOINFO mAlgoInfo;
    Mat processMat;
    int frameCount;
    void sortPointAscend(vector <Point> &points, int length, int flagX);
    vector<vector<Point>> innerMaskRect(RotatedRect minRectTmpParam);
    void getRadRectBinInfor(Mat rectBinMat, radRectInfors &resTmp);
    Mat getStandMask(RotatedRect rectParam, Mat binMat);
    int cmpRectImg(Mat findBinImg, Mat standBinImg);
    bool checkDiffRectPix(int diffCount);
    void pixsConvertLength(rectInfors rectHW, float &widthPara, float &heighPara);
};

#endif // STATIONONEPROCESSOR_HPP
