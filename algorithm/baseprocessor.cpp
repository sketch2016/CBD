#include "include/algorithm/baseprocessor.hpp"
#include "include/util/logger.h"

#define TAG "BaseProcessor"

void BaseProcess::swapValue(int &paraA, int &paraB)
{
    int tmp;
    if (paraA > paraB)
    {
        tmp = paraA;
        paraB = paraA;
        paraA = tmp;
    }
}

Mat BaseProcess::getScaleImg(Mat srcImg, int scaleParam)
{
    if (srcImg.empty())
    {
        LOGE(TAG, "error BaseProcess  the getScaleImg is empty");
    }

    Mat scaleImg;
    int  scaleThreshs = scaleParam;
    int scaleImgWidth = (srcImg.size().width)/scaleThreshs;
    int scaleImgHight = (srcImg.size().height)/scaleThreshs;
    resize(srcImg, scaleImg, Size(scaleImgWidth, scaleImgHight), 0, 0, INTER_LINEAR);

    return scaleImg;
}

Mat BaseProcess::getBinImg(Mat srcImg, int grayThresh)
{
    if (srcImg.empty())
    {
        LOGE(TAG, "error BaseProcess  the getBinImg is empty");
        //exit(1);
    }

    Mat grayImg, binaryImg, gauFilterImg;
    GaussianBlur(srcImg, gauFilterImg, Size(3, 3), 1.5);
    if (gauFilterImg.channels()==3)
    {
        cvtColor(gauFilterImg, grayImg, CV_BGR2GRAY);
    }
    else
    {
        grayImg = gauFilterImg;
    }
    threshold(grayImg, binaryImg, grayThresh, 255, THRESH_BINARY);
    return binaryImg;
}

Mat BaseProcess::getAdBinImg(Mat srcImg, int blockSize, int constValue)
{
    if (srcImg.empty())
    {
        LOGE(TAG, "error BaseProcess  the getAdBinImg is empty");
        exit(1);
    }

    Mat grayImg, binaryImg, gauFilterImg;
    GaussianBlur(srcImg, gauFilterImg, Size(3, 3), 1.0);
    if (gauFilterImg.channels()==3)
    {
        cvtColor(gauFilterImg, grayImg, CV_BGR2GRAY);
    }
    else
    {
        grayImg = gauFilterImg;
    }
    /* 0: THRESH_BINARY
     * 1: THRESH_BINARY_INV
    */
    const int maxVal = 255;
    adaptiveThreshold(grayImg, binaryImg, maxVal, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, blockSize, constValue);

    return binaryImg;
}

Mat BaseProcess::getHSVBinImg(Mat matImg, Scalar hsvPara, Scalar HSVPara)
{
    int h =hsvPara.val[0]; int H =HSVPara.val[0];
    int s =hsvPara.val[1]; int S =HSVPara.val[1];
    int v =hsvPara.val[2]; int V =HSVPara.val[2];

    bool condH = (h <0)||(H > 180);
    bool condS = (s <0)||(S > 255);
    bool condV = (v <0)||(V > 255);

    if(condH|condS|condV)
    {
        LOGE(TAG, "getHSVBinImg the config is error.");
    }

    Mat srcHsvImg, resMask;
    cvtColor(matImg, srcHsvImg, COLOR_BGR2HSV);
    inRange(srcHsvImg, Scalar(h, s, v), Scalar(H, S, V), resMask);

//    imshow("srcHsvImg", srcHsvImg);
//    imshow("resMask", resMask);
//    waitKey(0);

    return resMask;
}

Mat BaseProcess::getBGRBinImg(Mat matImg, Scalar bgrPara, Scalar BGRPara)
{
    int b =bgrPara.val[0]; int B =BGRPara.val[0];
    int g =bgrPara.val[1]; int G =BGRPara.val[1];
    int r =bgrPara.val[2]; int R =BGRPara.val[2];

    bool condB = (b <0)||(B > 255);
    bool condG = (g <0)||(G > 255);
    bool condR = (r <0)||(R > 255);
    if(condB|condG|condR)
    {
		LOGE(TAG, "getBGRBinImg the config is error.");
    }

    Mat resMask;
    inRange(matImg, Scalar(b, g, r), Scalar(B, G, R), resMask);

//    imshow("srcHsvImg", srcHsvImg);
//    imshow("resMask", resMask);
//    waitKey(0);

    return resMask;
}
Mat BaseProcess::getRoiImg(Mat srcImg, Rect roiRect)
{
    int imgMaxHight = srcImg.size().height;
    int imgMaxWidth = srcImg.size().width;

    int widthIndexEnd = roiRect.tl().x + roiRect.width;
    int hightIndexEnd = roiRect.tl().y + roiRect.height;

    if((widthIndexEnd > imgMaxWidth)||(hightIndexEnd > imgMaxHight))
    {
        LOGE(TAG, "the BaseProcess::getRoiImg index is error");
    }

    Mat rectRoiImg;
    rectRoiImg = srcImg(Rect(roiRect.tl().x, roiRect.tl().y, roiRect.width, roiRect.height));

    return rectRoiImg;
}

float BaseProcess::blackCountRation(Mat openBinyImg, int scaleThreshs)
{
    Mat scaleBinImg;
    int scaleImgWidth = (openBinyImg.size().width)/scaleThreshs;
    int scaleImgHight = (openBinyImg.size().height)/scaleThreshs;
    resize(openBinyImg, scaleBinImg, Size(scaleImgWidth, scaleImgHight), 0, 0, INTER_LINEAR);

    int blackCount = 0; int whiteCout =0;
    int scaleImgWidths = scaleBinImg.size().width;
    int scaleImgHeighs= scaleBinImg.size().height;
    for (int i=0; i<scaleImgWidths; i++)
    {
        for (int j=0; j<scaleImgHeighs; j++)
        {
            if (scaleBinImg.at<uchar>(i, j) ==0)
            {
                blackCount = blackCount +1;
            }
            else
            {
                whiteCout = whiteCout+1;
            }
        }
    }

    float openRation = float(blackCount)/float(scaleImgWidths*scaleImgHeighs);
    //cout<<"StatOnePross open check:  the black and white is: "<<blackCount<<"---"<<whiteCout<<"---"<<openRation<<endl;

    return openRation;
}

bool BaseProcess::getOpenCheck(Mat srcMatImg, int openGrayThreshs, float openStatueRation)
{
    Mat openBinImg = getBinImg(srcMatImg, openGrayThreshs);
    float allPixCounts = openBinImg.size().width *openBinImg.size().height;
    float whitePixCounts = countNonZero(openBinImg);

//    imshow("openBinImg", openBinImg);
//    waitKey(0);

    if (allPixCounts == 0)
    {
        return false;
    }
    float findOpenRaton = whitePixCounts/allPixCounts;
    if(findOpenRaton > openStatueRation)
    {
        //cout<<"the openStatue is ok ---"<<whiteCountRations<<endl;
        return true;
    }
    else
    {
        //cout<<"the openStatue is closed--"<<whiteCountRations<<endl;
        return false;
    }
}

Mat BaseProcess::getUndistPersp(Mat srcImg, Mat camMat, Mat distMat, Mat perspMat)
{
    if(srcImg.empty() || camMat.empty() || distMat.empty() || perspMat.empty())
    {
        LOGE(TAG, "BaseProcess getUndistPersp config mat is error");
        //exit(1);
    }

    Mat unDistImg;
    undistort(srcImg, unDistImg, camMat, distMat);
    Mat unDistPerspImg;
    warpPerspective(unDistImg, unDistPerspImg, perspMat, srcImg.size());

    return unDistPerspImg;
}
