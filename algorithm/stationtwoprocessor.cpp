#include "include/algorithm/stationtwoprocessor.hpp"
#include "include/util/logger.h"

#define TAG "StationTwoProcessor"
using namespace std;

bool StatusTwoprocess::getOpenStatus(Mat srcMatImg)
{
    int openGrayThreshs = mAlgoInfo.openGrayThresh2;
    float openStatueRation = mAlgoInfo.openFillRation2;

    bool getOpenFlag = getOpenCheck(srcMatImg, openGrayThreshs, openStatueRation);
    //cout<<"StatTwoPross the openStatue is ---"<<getOpenFlag<<endl;

    return getOpenFlag;
}

bool StatusTwoprocess::getLapStatus(Mat srcMatImg)
{
    bool checkLapRes = true;
    Mat rectRoiImg = getInRectROI(srcMatImg);
    Mat rectAccRoiImg = getAccInRectROI(rectRoiImg);
    Mat lapBinImg = getWhiteBinImg(rectAccRoiImg);
    Mat lapMorImg = getWhiteMorImg(lapBinImg);

    vector<Vec4i> hierarchy;
    vector<vector<Point> > contours;
    findContours(lapMorImg, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);  //RETR_LIST  RETR_TREE
    if (contours.size() == 0)
    {
        LOGE(TAG, "getHgzInfors not find the contour exit");
        return false;
    }

    vector<int> sortIdx(contours.size());
    vector<float> areas(contours.size());
    for (int n=0; n<(int)contours.size(); n++)
    {
       sortIdx[n] =n;
       areas[n] =contourArea(contours[n], false);
    }
    std::sort(sortIdx.begin(), sortIdx.end(), AreaCmp(areas));

    float LengthScale = mAlgoInfo.lapLengthScale;
    float pixBaseRation = mAlgoInfo.pixBaseRation2;
    float findLapLengthMax = (mAlgoInfo.hgzWidth);
    float cmpLengthStand = pixBaseRation*findLapLengthMax*LengthScale;

    float AreaScale = mAlgoInfo.lapAreaScale;
    int hgzBaseArea = (mAlgoInfo.hgzHight)*(mAlgoInfo.hgzWidth);
    float imageMaxAreaStand = hgzBaseArea*pixBaseRation*pixBaseRation*AreaScale;

    int swapChangeTmp;
    for (int i=0; i<contours.size(); i++)
    {
        int idx = sortIdx[i];
        RotatedRect minRectTmp = minAreaRect(contours[idx]);
        int findHgzWidth = int(minRectTmp.size.width);
        int findHgzHeight = int(minRectTmp.size.height);

        if(findHgzHeight > findHgzWidth)
        {
            swapChangeTmp = findHgzHeight;
            findHgzHeight = findHgzWidth;
            findHgzWidth = swapChangeTmp;
        }

        if (findHgzWidth>60 && findHgzHeight >50)
        {
            //std::cout<<"hgz find width and heigh is: "<<findHgzWidth<<"  "<<findHgzHeight<<endl;
        }
        else
        {
            continue;
        }

//        Mat drawTmp = rectAccRoiImg.clone();
//        drawContours(drawTmp, contours, idx, Scalar(255, 255, 255), -1);
//        imshow("drawTmp", drawTmp);
//        waitKey(0);

        //the area is bigger than the stand area
        if(areas[idx] > imageMaxAreaStand)
        {
            LOGE(TAG, "a.a the overlap area check is found");
            checkLapRes = false;
            break;
        }
        //the length is extern than the stand length
        if(findHgzWidth > cmpLengthStand)
        {
            LOGE(TAG, "a.a the overlap length check is found");
            checkLapRes = false;
            break;
        }

    }

    return checkLapRes;
}

Mat StatusTwoprocess::getHgzMorImg(Mat srcMorImg)
{
    Mat morImg;
    if (srcMorImg.channels() == 3)
    {
        cvtColor(srcMorImg, morImg, CV_BGR2GRAY);
    }
    else
    {
        morImg = srcMorImg;
    }

    Mat elementEro=getStructuringElement(MORPH_ELLIPSE, Size(3,3));
    erode(morImg, morImg, elementEro, Point(-1,-1), 1);
    Mat elementDia=getStructuringElement(MORPH_ELLIPSE, Size(3,3));
    dilate(morImg, morImg, elementDia, Point(-1,-1), 1);
    // get the connect binary image
    Mat elementDia2=getStructuringElement(MORPH_ELLIPSE, Size(5,5));
    dilate(morImg, morImg, elementDia2, Point(-1,-1), 3);
    Mat elementEro2=getStructuringElement(MORPH_ELLIPSE, Size(5,5));
    erode(morImg, morImg, elementEro2, Point(-1,-1), 3);

    return morImg;
}


bool StatusTwoprocess::CalHgzBlackPix(Mat hgzRoiImg, Mat hgzMaskImg, int findHgzWidth, int findHgzHeight)
{
    int rValue = mAlgoInfo.hgzBlackr; int RValue = mAlgoInfo.hgzBlackR;
    int gValue = mAlgoInfo.hgzBlackg; int GValue = mAlgoInfo.hgzBlackG;
    int bValue = mAlgoInfo.hgzBlackb; int BValue = mAlgoInfo.hgzBlackB;
    Scalar BlackhsvPara(rValue, gValue, bValue);
    Scalar BlackHSVPara(RValue, GValue, BValue);

    Mat hgzExactRoiImg;
    hgzRoiImg.copyTo(hgzExactRoiImg, hgzMaskImg);
    Mat hgzBlackImg = getBGRBinImg(hgzExactRoiImg, BlackhsvPara, BlackHSVPara);

    Mat hgzExactBlackImg;
    hgzBlackImg.copyTo(hgzExactBlackImg, hgzMaskImg);

    Mat hgzExactBlackImgMor;
    Mat elementDia = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
    dilate(hgzExactBlackImg, hgzExactBlackImgMor, elementDia, Point(-1,-1), 1);

//    imshow("hgzExactBlackImgMor", hgzExactBlackImgMor);
//    waitKey(0);
//    destroyAllWindows();

    float blackPixCount = countNonZero(hgzExactBlackImgMor);
    float gzjPixCounts = findHgzWidth*findHgzHeight;
    float blackPixRation = blackPixCount/gzjPixCounts;
    float hgzStandBlackRat = mAlgoInfo.hgzBlackRation;

    if(blackPixRation < hgzStandBlackRat)
    {
        LOGE(TAG, "error the statusTwo hgz black block is error");
        return false;
    }
    else
    {
        return true;
    }
}

bool StatusTwoprocess::CalHgzRedPix(Mat hgzRoiImg, Mat hgzMaskImg, int findHgzWidth, int findHgzHeight)
{
    int hValue = mAlgoInfo.hgzRedh; int HValue = mAlgoInfo.hgzRedH;
    int sValue = mAlgoInfo.hgzReds; int SValue = mAlgoInfo.hgzRedS;
    int vValue = mAlgoInfo.hgzRedv; int VValue = mAlgoInfo.hgzRedV;
    Scalar redhsvPara(hValue, sValue, vValue);
    Scalar redHSVPara(HValue, SValue, VValue);

    Mat hgzExactRoiImg;
    hgzRoiImg.copyTo(hgzExactRoiImg, hgzMaskImg);
    Mat hgzRedImg = getHSVBinImg(hgzExactRoiImg, redhsvPara, redHSVPara);
    Mat elementDia=getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
    dilate(hgzRedImg, hgzRedImg, elementDia, Point(-1,-1), 2);

//    imshow("hgzExactRoiImg", hgzExactRoiImg);
//    imshow("hgzRedImghaha  ", hgzRedImg);
//    waitKey(0);
//    destroyAllWindows();

    float redPixCount = countNonZero(hgzRedImg);
    float gzjPixCounts = findHgzWidth*findHgzHeight;
    float redPixRation = redPixCount/gzjPixCounts;
    float hgzStandRedRat = mAlgoInfo.hgzRedRation;

    if(redPixRation < hgzStandRedRat)
    {
        LOGE(TAG, "error the statusTwo hgz red is error");
        return false;
    }
    else
    {
        return true;
    }
}

bool StatusTwoprocess::checkWhiteRegionCount(Mat binMorImg)
{
    bool checkRes = true;
    vector<Vec4i> hierarchy;
    vector<vector<Point> > contours;
    findContours(binMorImg, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);  //RETR_LIST  RETR_TREE
    if (contours.size() == 0)
    {
        LOGE(TAG, "checkWhiteRegionCount not find the contour exit");
        checkRes = false;
        return checkRes;
    }
    vector<int> sortIdx(contours.size());
    vector<float> areas(contours.size());
    for (int n=0; n< (int)contours.size(); n++)
    {
       sortIdx[n] =n;
       areas[n] = contourArea(contours[n], false);
    }
    std::sort(sortIdx.begin(), sortIdx.end(), AreaCmp(areas));

    float pixBaseRation = mAlgoInfo.pixBaseRation2;
    int minWidth = min(mAlgoInfo.hgzWidth, mAlgoInfo.gzjWidth);
    int minHight = min(mAlgoInfo.hgzHight, mAlgoInfo.gzjHight);
    int minLength = 0.9*pixBaseRation*(min(minWidth, minHight));
    float minWhiteStandArea = minLength*minLength;

    int oneCountFlag = 0;
    bool conWhite1, conWhite2;
    for (int i=0; i<contours.size(); i++)
    {
        int idx = sortIdx[i];
        RotatedRect minRectTmp = minAreaRect(contours[idx]);
        int findHgzWidth = int(minRectTmp.size.width);
        int findHgzHeight = int(minRectTmp.size.height);

        int swapChangeTmp;
        if(findHgzHeight > findHgzWidth)
        {
            swapChangeTmp = findHgzHeight;
            findHgzHeight = findHgzWidth;
            findHgzWidth = swapChangeTmp;
        }


        conWhite1 =(findHgzWidth > minLength);
        conWhite2 = (areas[idx] > minWhiteStandArea);

        if(conWhite1 &&conWhite2)
        {
            oneCountFlag = oneCountFlag +1;
        }
    }

    if (oneCountFlag > 1)
    {
        LOGE(TAG, "getHgzStatus find more then one");
        checkRes = false;

    }
    else if(oneCountFlag == 1)
    {
        checkRes = true;
    }
    else
    {
        checkRes = false;
    }

    return checkRes;
}


bool StatusTwoprocess::checkHgzRedBlackImg(Mat rectAccRoiImg, Mat hgzBinMorImg)
{
    bool checkRes;
    vector<Vec4i> hierarchy;
    vector<vector<Point> > contours;
    findContours(hgzBinMorImg, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);  //RETR_LIST  RETR_TREE
    if (contours.size() == 0)
    {
        LOGE(TAG, "checkHgzRedBlackImg not find the contour exit");
        checkRes = false;
        return checkRes;
    }
    vector<int> sortIdx(contours.size());
    vector<float> areas(contours.size());
    for (int n=0; n< (int)contours.size(); n++)
    {
       sortIdx[n] =n;
       areas[n] = contourArea(contours[n], false);
    }
    std::sort(sortIdx.begin(), sortIdx.end(), AreaCmp(areas));
    float pixBaseRation = mAlgoInfo.pixBaseRation2;
    float minScale = 0.8*pixBaseRation; float maxScale = 1.2*pixBaseRation;
    float findHgzHmin = (mAlgoInfo.hgzHight)*minScale; float findHgzHmax = (mAlgoInfo.hgzHight)*maxScale;
    float findHgzWmin = (mAlgoInfo.hgzWidth)*minScale; float findHgzWmax = (mAlgoInfo.hgzWidth)*maxScale;

    int swapChangeTmp; int firstHgzCount = 0;
    Mat hgzMaskImg = Mat::zeros(hgzBinMorImg.size(), hgzBinMorImg.type());
    for (int i=0; i<contours.size(); i++)
    {
        int idx = sortIdx[i];
        RotatedRect minRectTmp = minAreaRect(contours[idx]);
        int findHgzWidth = int(minRectTmp.size.width);
        int findHgzHeight = int(minRectTmp.size.height);

        if(findHgzHeight > findHgzWidth)
        {
            swapChangeTmp = findHgzHeight;
            findHgzHeight = findHgzWidth;
            findHgzWidth = swapChangeTmp;
        }

        if (findHgzWidth>60 && findHgzHeight >50)
        {
            //std::cout<<"hgz find width and heigh is: "<<findHgzWidth<<"  "<<findHgzHeight<<endl;
        }
        else
        {
            continue;
        }

        bool cond1 = (findHgzHeight > findHgzHmin)&&(findHgzHeight < findHgzHmax);
        bool cond2 = (findHgzWidth > findHgzWmin)&&(findHgzWidth < findHgzWmax);
        if(cond1 &&cond2)
        {
            Mat hgzMaskImgTmp = hgzMaskImg.clone();Mat morImg;
            drawContours(hgzMaskImgTmp, contours, idx, Scalar(255, 255, 255), -1);
            Mat elementEroTmp=getStructuringElement(MORPH_ELLIPSE, Size(3,3));
            erode(hgzMaskImgTmp, morImg, elementEroTmp, Point(-1,-1), 1);

            bool resCalHgzRed = CalHgzRedPix(rectAccRoiImg, morImg, findHgzWidth, findHgzHeight);
            bool resCalHgzBlack = CalHgzBlackPix(rectAccRoiImg, morImg, findHgzWidth, findHgzHeight);

            if ((resCalHgzRed == true)&&(resCalHgzBlack == true))
            {
                firstHgzCount = firstHgzCount +1;
                checkRes = true;
            }
            else
            {
                checkRes = false;
            }
        }
        else
        {
            checkRes = false;
        }

    }

    if(firstHgzCount > 1)
    {
        checkRes = false;
        LOGE(TAG,"error the statusTwo checkHgzRedBlackImg more one hgz");
    }
    else if(firstHgzCount == 1)
    {
        checkRes = true;
    }
    if(checkRes == false)
    {
       LOGE(TAG,"error the statusTwo checkHgzRedBlackImg is back");
    }

    return checkRes;
}
bool StatusTwoprocess::getHgzStatus(Mat srcMatImg)
{
    bool checkHgzRes = true;
    Mat rectRoiImg = getInRectROI(srcMatImg);
    Mat rectAccRoiImg = getAccInRectROI(rectRoiImg);
    Mat whiteBinImg = getWhiteBinImg(rectAccRoiImg);
    Mat hgzBinMorImg = getHgzMorImg(whiteBinImg);

//    imshow("hgzBinMorImg", hgzBinMorImg);
//    waitKey(0);
//    destroyAllWindows();

    // the white region is more then one, return error
    bool whiteCountFlag = checkWhiteRegionCount(hgzBinMorImg);
    if (whiteCountFlag == false)
    {
        checkHgzRes = false;
        return checkHgzRes;
    }

    //check the white region whether is hgz
    bool hgzRes = checkHgzRedBlackImg(rectAccRoiImg, hgzBinMorImg);
    if (hgzRes == false)
    {
        checkHgzRes = false;
        return checkHgzRes;
    }

    return checkHgzRes;
}


Mat StatusTwoprocess::getWhiteMorImg(Mat srcMorImg)
{
    Mat morImg;
    if (srcMorImg.channels() == 3)
    {
        cvtColor(srcMorImg, morImg, CV_BGR2GRAY);
    }
    else
    {
        morImg = srcMorImg;
    }

    Mat elementEro=getStructuringElement(MORPH_ELLIPSE, Size(3,3));
    erode(morImg, morImg, elementEro, Point(-1,-1), 1);
    Mat elementDia=getStructuringElement(MORPH_ELLIPSE, Size(3,3));
    dilate(morImg, morImg, elementDia, Point(-1,-1), 2);
    return morImg;
}

Mat StatusTwoprocess::getInRectROI(Mat srcMatImg)
{
    int rectTlx = mAlgoInfo.roiTlX2; int rectTly = mAlgoInfo.roiTlY2;
    int rectWidth = mAlgoInfo.roiWidth2; int rectHight = mAlgoInfo.roiHight2;
    Rect rectROI(rectTlx, rectTly, rectWidth, rectHight);
    Mat srcRoiIMg = getRoiImg(srcMatImg, rectROI);

    return srcRoiIMg;
}


Mat StatusTwoprocess::getWhiteBinImg(Mat rectRoiImg)
{
    int hValue = mAlgoInfo.whiteh; int HValue = mAlgoInfo.whiteH;
    int sValue = mAlgoInfo.whites; int SValue = mAlgoInfo.whiteS;
    int vValue = mAlgoInfo.whitev; int VValue = mAlgoInfo.whiteV;

    Scalar hsvPara(hValue, sValue, vValue);
    Scalar HSVPara(HValue, SValue, VValue);
    Mat whiteBinImg = getHSVBinImg(rectRoiImg, hsvPara, HSVPara);

    return whiteBinImg;
}

Mat StatusTwoprocess::getAccInRectROI(Mat srcMatImg)
{
    int grayValue = 10; Mat grayImg;
    if (srcMatImg.channels()==3)
    {
        cvtColor(srcMatImg, grayImg, CV_BGR2GRAY);
    }
    else
    {
        grayImg = srcMatImg;
    }
    Mat binaryImg;
    threshold(grayImg, binaryImg, grayValue, 255, THRESH_BINARY);


    int widthBlackCount[binaryImg.size().width]={0};
    for(int i=0; i<binaryImg.size().width; i++)
    {
        Mat colLine = binaryImg.col(i);
        widthBlackCount[i] = countNonZero(colLine);
        //cout<<"index is: "<<i<<"  "<<widthBlackCount[i]<<endl;
    }
    int minStand = (mAlgoInfo.rectHight)*(mAlgoInfo.pixBaseRation2);

    bool inBlock =false;
    int beginIndex =0; int endIndex =0;
    for(int i=0; i<binaryImg.size().width-4;i++)
    {
        if((!inBlock)&&(widthBlackCount[i]>minStand)&&(widthBlackCount[i+1]>minStand)&&(widthBlackCount[i+2]>minStand)&&(widthBlackCount[i+3]>minStand)&&(widthBlackCount[i+4]>minStand))
        {
            inBlock = true;
            beginIndex =i;
        }
        else if((inBlock)&&(widthBlackCount[i]<minStand)&&(widthBlackCount[i+1]<minStand)&&(widthBlackCount[i+2]<minStand)&&(widthBlackCount[i+3]<minStand)&&(widthBlackCount[i+4]<minStand))
        {
            inBlock = false;
            endIndex =i;
            break;
        }
    }

    int deltaWidth = endIndex -beginIndex;
    int deltaWidthStand = (mAlgoInfo.pixBaseRation2 *mAlgoInfo.rectWidth)*0.8;
    if (deltaWidth <  deltaWidthStand)
    {
        LOGE(TAG, "getAccInRectROI error");

    }
//    cout<<"the begin and end is: "<<beginIndex<<" "<<endIndex<<endl;
    Mat resMat = srcMatImg(Range(0,srcMatImg.size().height), Range(beginIndex, endIndex));
//    imshow("yayayresMat", resMat);
//    waitKey(0);
    return resMat;

}

Mat StatusTwoprocess::getSdMorImg(Mat srcMorImg)
{
    Mat morImg;
    if (srcMorImg.channels() == 3)
    {
        cvtColor(srcMorImg, morImg, CV_BGR2GRAY);
    }
    else
    {
        morImg = srcMorImg;
    }

//    Mat elementEro=getStructuringElement(MORPH_RECT, Size(3,3));
//    erode(morImg, morImg, elementEro, Point(-1,-1), 1);
    Mat elementDia=getStructuringElement(MORPH_RECT, Size(3,3));
    dilate(morImg, morImg, elementDia, Point(-1,-1), 2);
    return morImg;
}

Mat StatusTwoprocess::getSdBinImg(Mat srcImg)
{
//    int hValue = mConfig->sdh; int HValue = mConfig->sdH;
//    int sValue = mConfig->sds; int SValue = mConfig->sdS;
//    int vValue = mConfig->sdv; int VValue = mConfig->sdV;
//    Scalar hsvPara(hValue, sValue, vValue);
//    Scalar HSVPara(HValue, SValue, VValue);
//    Mat sdBinImg = getColorBinImg(srcImg, hsvPara, HSVPara);

    int grayValue = mAlgoInfo.sdGrayThresh;
    Mat sdBinImg =getBinImg(srcImg, grayValue);
//    imshow("sdBinImg", sdBinImg);
//    waitKey(0);

    return sdBinImg;
}
Mat StatusTwoprocess::getSdRoiImg(Mat srcImg)
{
    int sdROItlX = mAlgoInfo.sdRoiTlX;
    int sdROItlY = mAlgoInfo.sdRoiTlY;
    int sdROIWidth = mAlgoInfo.sdRoiWidth;
    int sdROIHight = mAlgoInfo.sdRoiHight;

    int imgBaseHight = srcImg.size().height;
    int imgBaseWidth = srcImg.size().width;

    int widthIndexEnd = sdROIWidth + sdROItlX;
    int hightIndexEnd = sdROIHight + sdROItlY;
    if((widthIndexEnd > imgBaseWidth)||(hightIndexEnd > imgBaseHight))
    {
        LOGE(TAG, "the StatusTwoprocess::getSdRoiImg index is error");

    }
    Mat rectRoiImg;
    rectRoiImg = srcImg(Rect(sdROItlX, sdROItlY, sdROIWidth, sdROIHight));

    return rectRoiImg;
}

int StatusTwoprocess::getSdWidth(Mat sdRoiImg)
{
    int sdRowPixCount[sdRoiImg.size().height] = {0};
    Mat elementDiaSd=getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
    dilate(sdRoiImg, sdRoiImg, elementDiaSd, Point(-1,-1), 2);
    Mat element=getStructuringElement(MORPH_ELLIPSE, Size(3,3));
    erode(sdRoiImg, sdRoiImg, element, Point(-1,-1), 2);

//    imshow("sdRoiImg", sdRoiImg);
//    waitKey(0);
//    destroyAllWindows();

    for(int i =0; i<sdRoiImg.size().height; i++)
    {
        Mat sdRoiRow = sdRoiImg.row(i);
        int whiteCount = countNonZero(sdRoiRow);
        sdRowPixCount[i] = whiteCount;
        //cout<<"the StatusTwoprocess::getSdWidth is: "<<i<<"  "<<sdRowPixCount[i]<<endl;
    }

    int countRowThreshMax = (mAlgoInfo.sdBgLength)*(mAlgoInfo.pixBaseRation2);
    int countRowThreshMin = 10;
    bool inBlockRow = false;
    int rowBeginIndex =0; int rowEndIndex = 0;
    for(int j=0; j<sdRoiImg.size().height-3; j++)
    {
        if(!inBlockRow &&(sdRowPixCount[j] < countRowThreshMax)&&(sdRowPixCount[j+1] < countRowThreshMax)&&(sdRowPixCount[j+2] < countRowThreshMax)&&(sdRowPixCount[j+3] < countRowThreshMax))
        {
            inBlockRow = true;
            rowBeginIndex = j;
            //cout<<"the row startIndex is: "<<rowBeginIndex<<endl;
        }
        else if(inBlockRow&&(sdRowPixCount[j] < countRowThreshMin)&&(sdRowPixCount[j+1] < countRowThreshMin)&&(sdRowPixCount[j+2] < countRowThreshMin)&&(sdRowPixCount[j+3] < countRowThreshMin))
        {
            inBlockRow = false;
            rowEndIndex = j;
            //cout<<"the row endIndex is: "<<rowEndIndex<<endl;
            break;
        }
    }
    Mat sdRoiRect = sdRoiImg(Range(rowBeginIndex, rowEndIndex), Range(0,sdRoiImg.size().width));

//    imshow("sdRoiRect", sdRoiRect);
//    waitKey(0);
//    destroyAllWindows();

    int sdColPixCount[sdRoiRect.size().width] = {0};
    for(int i =0; i<sdRoiRect.size().width; i++)
    {
        Mat sdRoiCol = sdRoiRect.col(i);
        int whiteCount = countNonZero(sdRoiCol);
        sdColPixCount[i] = whiteCount;
        //cout<<"the StatusTwoprocess::getSdWidth is: "<<i<<"  "<<sdColPixCount[i]<<endl;
    }

    int countThresh = 5;
    bool inBlock = false;
    int findSdBegin =0, findSdEnd =0;
    bool firstStartFlag = false; bool firstEndFlag = false;
    for(int i=0; i<sdRoiImg.size().width-3; i++)
    {
        if (!inBlock && (sdColPixCount[i]< countThresh) &&(sdColPixCount[i+1]< countThresh)&&(sdColPixCount[i+2]< countThresh)&&(sdColPixCount[i+3]< countThresh))
        {
            inBlock = true;
            if (firstStartFlag == false)
            {
                firstStartFlag = true;
                continue;
            }
            findSdBegin = i;
            //cout<<"the sd lenght startIndex is: "<<findSdBegin<<endl;
        }
        else if(inBlock && (sdColPixCount[i]> countThresh) && (sdColPixCount[i+1]> countThresh)&& (sdColPixCount[i+2]> countThresh)&& (sdColPixCount[i+3]> countThresh))
        {
            inBlock = false;
            if (firstEndFlag == false)
            {
                firstEndFlag = true;
                continue;
            }
            findSdEnd = i;
            //cout<<"the hight endIndex is: "<<findSdEnd<<endl;
            break;
        }
    }

    int findSdWidth = findSdEnd - findSdBegin;
    //cout<<"hahahaha findSdWidth is: "<<findSdWidth<<endl;
    return findSdWidth;
}


bool StatusTwoprocess::getSdStatus(Mat srcMatImg)
{
    bool checkSdRes = true;
    Mat sdRoiImg = getSdRoiImg(srcMatImg);
    Mat sdBinImg = getSdBinImg(sdRoiImg);
    Mat sdMorImg = getSdMorImg(sdBinImg);

//    imshow("sdMorImg", sdMorImg);
//    waitKey(0);
//    destroyAllWindows();

    int findSdWidth = getSdWidth(sdMorImg);
    int standSdWidth = (mAlgoInfo.sdLength)*(mAlgoInfo.pixBaseRation2);
    if (findSdWidth > standSdWidth)
    {
        return true;
    }
    else
    {
        return false;
    }
}

statTwoRes StatusTwoprocess::doCheckStatusTwo(Mat srcBGRImg)
{
    statTwoRes resTmp;
    resTmp.stationOkFlag = false;

    if (srcBGRImg.empty())
    {
        LOGE(TAG, "error, the StatusTwoprocess::doCheckStatusTwo is empty");
        return resTmp;
    }
    Mat camMat = mAlgoInfo.camMatrix2;
    Mat distMat = mAlgoInfo.distMatrix2;
    Mat perspMat = mAlgoInfo.perspFromMatrix2;
    Mat unDistPerspImg = getUndistPersp(srcBGRImg, camMat, distMat, perspMat); //get the distort and pespective image
    bool openStatusFlag = getOpenStatus(unDistPerspImg);                             // the box is open
    LOGD(TAG,"a:  openStatusFlag is: %1", openStatusFlag);

    bool lapStatusFlag, hgzStatusFlag, sdStatusFlag;
    if(openStatusFlag == true)
    {
        lapStatusFlag = getLapStatus(unDistPerspImg);
        LOGD(TAG,"b: lapStatusFlag is: %1", lapStatusFlag);
        if (lapStatusFlag == true)
        {
            hgzStatusFlag = getHgzStatus(unDistPerspImg);
            LOGD(TAG,"c: hgzStatusFlag is: %1", hgzStatusFlag);
            if(hgzStatusFlag == true)
            {
                sdStatusFlag = getSdStatus(unDistPerspImg);
                LOGD(TAG,"d: sdStatusFlag is: %1", sdStatusFlag);
                if(sdStatusFlag == true)
                {
                    resTmp.stationOkFlag = true;
                }
                else
                {
                    resTmp.stationOkFlag = false;
                    resTmp.errCode.push_back(SilkError);
                }
            }
            else  //the hgz case
            {
                resTmp.stationOkFlag = false;
                resTmp.errCode.push_back(CertificateError);
            }
        }
        else  //the overlap case
        {
            resTmp.stationOkFlag = false;
            resTmp.errCode.push_back(CertificateError);
        }
    }
    else   //the open  case
    {
        resTmp.stationOkFlag = false;
        resTmp.errCode.push_back(StationTwoNotOpenError);
    }

    return resTmp;
}










