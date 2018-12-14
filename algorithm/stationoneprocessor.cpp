#include "include/algorithm/stationoneprocessor.hpp"
#include "include/util/logger.h"

#define TAG "StationOneProcessor"

bool StatOnePross::getOpenStatue(Mat srcMatImg)
{
    int openGrayThreshs = mAlgoInfo.openGrayThresh1;
    float openStatueRation = mAlgoInfo.openFillRation1;

    bool getOpenFlag = getOpenCheck(srcMatImg, openGrayThreshs, openStatueRation);
    //cout<<"StatOnePross the openStatue is ---"<<getOpenFlag<<endl;

    return getOpenFlag;
}

void StatOnePross::sortPointAscend(vector <Point> &points, int length, int flagX)
{
    Point tmp;
    for (int i=0; i<length -1; i++)
    {
        for(int j =0; j<length-1-i; j++)
        {
            if(flagX == 1)
            {
                if (points[j].x > points[j+1].x)
                {
                    tmp = points[j+1];
                    points[j+1] = points[j];
                    points[j] = tmp;
                }
            }
            else
            {
                if (points[j].y > points[j+1].y)
                {
                    tmp = points[j+1];
                    points[j+1] = points[j];
                    points[j] = tmp;
                }
            }
        }
    }
}


vector<vector<Point>> StatOnePross::innerMaskRect(RotatedRect minRectTmpParam)
{
    float removeRation = 0.1;
    CvPoint2D32f Corners[4];
    cvBoxPoints(minRectTmpParam, Corners);
    vector<Point>rectTmp;
    for (int i =0; i<4; i++)
    {
        rectTmp.push_back(Corners[i]);
    }

    sortPointAscend(rectTmp, 4, 1);
    vector <Point> leftMostPoints;
    leftMostPoints.push_back( rectTmp.at(0));
    leftMostPoints.push_back( rectTmp.at(1));
    sortPointAscend(leftMostPoints, 2, 0);  // tl bl

    vector <Point> rightMostPoints;
    rightMostPoints.push_back( rectTmp.at(2));
    rightMostPoints.push_back( rectTmp.at(3));
    sortPointAscend(rightMostPoints, 2, 0);  //tr br

    vector<Point> scalePoints;
    vector<vector<Point>> maskRectPoints;
    int rectWidth = minRectTmpParam.size.width;
    int rectHight = minRectTmpParam.size.height;
    if(rectWidth < rectHight)
    {
        int tmp = rectHight;
        rectHight = rectWidth;
        rectWidth = tmp;
    }

    int delatX = (rectWidth*removeRation); int delatY = (rectHight*removeRation);
    leftMostPoints.at(0).x = leftMostPoints.at(0).x +delatX;
    leftMostPoints.at(0).y = leftMostPoints.at(0).y +delatY;  //tl
    scalePoints.push_back(leftMostPoints.at(0));
    rightMostPoints.at(0).x = rightMostPoints.at(0).x -delatX;
    rightMostPoints.at(0).y = rightMostPoints.at(0).y +delatY;  //tr
    scalePoints.push_back(rightMostPoints.at(0));
    rightMostPoints.at(1).x = rightMostPoints.at(1).x -delatX;
    rightMostPoints.at(1).y = rightMostPoints.at(1).y -delatY;  //br
    scalePoints.push_back(rightMostPoints.at(1));
    leftMostPoints.at(1).x = leftMostPoints.at(1).x +delatX;
    leftMostPoints.at(1).y = leftMostPoints.at(1).y -delatY;  //bl
    scalePoints.push_back(leftMostPoints.at(1));

    maskRectPoints.push_back(scalePoints);
    return maskRectPoints;
}

void StatOnePross::getRadRectBinInfor(Mat rectBinMat, radRectInfors &resultTmp)
{
    vector<Vec4i> hierarchy;
    vector<vector<Point> > contours;
    findContours(rectBinMat, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);  //RETR_LIST  RETR_TREE

    resultTmp.radResFlag = false;
    if (contours.size() == 0)
    {
        LOGD(TAG, "subRectRes not find the contour exit");
        return;
    }

    vector<int> sortIdx(contours.size());
    vector<float> areas(contours.size());
    for (int n = 0; n < (int)contours.size(); n++)
    {
       sortIdx[n] = n;
       areas[n] = contourArea(contours[n], false);
    }
    std::sort(sortIdx.begin(), sortIdx.end(), AreaCmp(areas));

    float pixBaseRation = mAlgoInfo.pixBaseRation1;
    float minScale = 0.9*pixBaseRation; float maxScale = 1.1*pixBaseRation;
    float findRectHmin = (mAlgoInfo.rectHight)*minScale; float findRectHmax = (mAlgoInfo.rectHight)*maxScale;
    float findRectWmin = (mAlgoInfo.rectWidth)*minScale; float findRectWmax = (mAlgoInfo.rectWidth)*maxScale;

    RotatedRect minRectTmp;  //findRectBinImg
    Mat findRectImg = Mat::zeros(rectBinMat.size(), rectBinMat.type());
    for (int i=0; i<contours.size(); i++)
    {
        int idx = sortIdx[i];
        minRectTmp = minAreaRect(contours[idx]);

        int findRectWidth = int(minRectTmp.size.width);
        int findRectHeight = int(minRectTmp.size.height);
        if (findRectWidth < findRectHeight)
        {
            int tmp =findRectWidth;
            findRectWidth = findRectHeight;
            findRectHeight = tmp;
        }

        if (findRectWidth>500 && findRectHeight >200)
        {
            //std::cout<<"radius  rect find width and heigh is: "<<findRectWidth<<"  "<<findRectHeight<<endl;
        }

        bool cond1 = (findRectHeight > findRectHmin)&&(findRectHeight < findRectHmax);
        bool cond2 = (findRectWidth > findRectWmin)&&(findRectWidth < findRectWmax);
        if(cond1 &&cond2)
        {
            resultTmp.radResFlag = true;
            drawContours(findRectImg, contours, idx, Scalar(255, 255, 255), -1);
//            imshow("findRectImg", findRectImg);
//            waitKey(0);
            break;
        }
    }

    if (resultTmp.radResFlag == true)
    {
        resultTmp.radRotRect = minRectTmp;
        resultTmp.findRectBinImg = findRectImg.clone();
    }

}

Mat StatOnePross::getStandMask(RotatedRect radRect, Mat sizeImg)
{
    Point2f radCentor = radRect.center;
    double radRotAngle = radRect.angle;
    float pixRation = mAlgoInfo.pixBaseRation1;
    int standRectWidth = pixRation * mAlgoInfo.rectWidth;
    int standRectHight = pixRation * mAlgoInfo.rectHight;

    int radRectWidth = radRect.size.width;
    int radRectHeight = radRect.size.height;
    if(radRectWidth < radRectHeight)
    {
       radRotAngle = radRotAngle+90;
    }
    /*
     *the width is bigger than heigh, abs(angle)
     * the width is less than height, 90+angle then invert
    */
    radRotAngle = -radRotAngle;
    Mat rotMat = getRotationMatrix2D(radCentor, radRotAngle, 1.0);

    int tlX = radCentor.x - standRectWidth/2;
    int tlY = radCentor.y - standRectHight/2;
    int trX = radCentor.x + standRectWidth/2;
    int trY = radCentor.y - standRectHight/2;
    int brX = radCentor.x + standRectWidth/2;
    int brY = radCentor.y + standRectHight/2;
    int blX = radCentor.x - standRectWidth/2;
    int blY = radCentor.y + standRectHight/2;

    Mat tlPoint =Mat::ones(3, 1, CV_64FC1);
    Mat trPoint =Mat::ones(3, 1, CV_64FC1);
    Mat brPoint =Mat::ones(3, 1, CV_64FC1);
    Mat blPoint =Mat::ones(3, 1, CV_64FC1);

    tlPoint.at<double>(0, 0) = tlX;   tlPoint.at<double>(1, 0) = tlY;
    trPoint.at<double>(0, 0) = trX;   trPoint.at<double>(1, 0) = trY;
    brPoint.at<double>(0, 0) = brX;   brPoint.at<double>(1, 0) = brY;
    blPoint.at<double>(0, 0) = blX;   blPoint.at<double>(1, 0) = blY;

    Mat dstTlPoint = rotMat*tlPoint;  Point tlCorner = Point(dstTlPoint.at<double>(0,0), dstTlPoint.at<double>(1,0));
    Mat dstTrPoint = rotMat*trPoint;  Point trCorner = Point(dstTrPoint.at<double>(0,0), dstTrPoint.at<double>(1,0));
    Mat dstBrPoint = rotMat*brPoint;  Point brCorner = Point(dstBrPoint.at<double>(0,0), dstBrPoint.at<double>(1,0));
    Mat dstBlPoint = rotMat*blPoint;  Point blCorner = Point(dstBlPoint.at<double>(0,0), dstBlPoint.at<double>(1,0));

//    just to debug
//    char pathTmp[100] = "/home/xiaoqi/work/tobaccoBox/qt/snap/mianguang5/test1.bmp";
//    Mat srcImg = imread(pathTmp);
//    circle(srcImg, tlCorner, 5, (0, 255, 0), 5);
//    circle(srcImg, trCorner, 5, (0, 255, 0), 5);
//    circle(srcImg, brCorner, 5, (0, 255, 0), 5);
//    circle(srcImg, blCorner, 5, (0, 255, 0), 5);

    vector<Point>rectFourPoints;
    rectFourPoints.push_back(tlCorner); rectFourPoints.push_back(trCorner);
    rectFourPoints.push_back(brCorner); rectFourPoints.push_back(blCorner);
    vector<vector<Point>> contoursRect;
    contoursRect.push_back(rectFourPoints);
    Mat standMaskImg = Mat::zeros(sizeImg.size(), sizeImg.type());
    drawContours(standMaskImg, contoursRect, -1, Scalar(255, 255, 255), -1);

//    imshow("standRectMaskImg", standMaskImg);
//    waitKey(0);

    return standMaskImg;

}

int StatOnePross::cmpRectImg(Mat findRectImg, Mat standRectImg)
{
    Mat  absDiffTmp;
    absdiff(findRectImg, standRectImg, absDiffTmp);

    int diffPixCounts = countNonZero(absDiffTmp);
//    imshow("findRectImg", findRectImg);
//    imshow("standRectImg", standRectImg);
//    imshow("absDiffTmp", absDiffTmp);
//    waitKey(0);

    return diffPixCounts;
}


bool StatOnePross::checkDiffRectPix(int diffCount)
{
  int worldWidth = mAlgoInfo.rectWidth;
  int worldHight = mAlgoInfo.rectHight;
  int pixRation = mAlgoInfo.pixBaseRation1;
  float checkRectScale = mAlgoInfo.rectDeltScale;

  int deltaPixStand = (worldWidth+worldHight)*checkRectScale*pixRation;

  if (diffCount < deltaPixStand)
  {
      return true;
  }
  else
  {
      return false;
  }

}

rectInfors StatOnePross:: getRectInfors(Mat srcMatImg)
{
    int rectGrayThresh;
    rectGrayThresh = mAlgoInfo.rectGrayThresh1;

    Mat rectBinImg = getBinImg(srcMatImg, rectGrayThresh);
    Mat element=getStructuringElement(MORPH_ELLIPSE, Size(3,3));
    erode(rectBinImg, rectBinImg, element, Point(-1,-1), 1);
    Mat elementDia=getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
    dilate(rectBinImg, rectBinImg, elementDia, Point(-1,-1), 1);

//    imshow("StatOnePross rough rectBinImg", rectBinImg);
//    waitKey(0);

    rectInfors findRectRes;
    findRectRes.rectOkFlag = false;
    findRectRes.heigh = -1;
    findRectRes.width = -1;

    radRectInfors radInforTmp;
    getRadRectBinInfor(rectBinImg, radInforTmp);

    bool doRectCheck = true;
    double diffPixsCount = 0;
    bool radInforResFlag = radInforTmp.radResFlag;
    if (radInforResFlag == true)
    {
        RotatedRect getRotRect = radInforTmp.radRotRect;
        Mat standRectMask = getStandMask(getRotRect, rectBinImg);
        Mat findRectMask = radInforTmp.findRectBinImg;
        diffPixsCount = cmpRectImg(findRectMask, standRectMask);
        doRectCheck = checkDiffRectPix(diffPixsCount);
        if (doRectCheck)
        {
            findRectRes.rectOkFlag = true;
        }
        else
        {
            findRectRes.rectOkFlag = false;
        }
    }
    else
    {
        findRectRes.rectOkFlag = false;
    }

    return findRectRes;
 }

void StatOnePross::pixsConvertLength(rectInfors rectHW, float &widthPara, float &heighPara)
{
    if ((rectHW.width == -1)||(rectHW.heigh == -1))
    {
        return;
    }

    float pixBaseRation = mAlgoInfo.pixBaseRation1;
    float widthParaTmp = rectHW.width/pixBaseRation;
    float heighParaTmp = rectHW.heigh/pixBaseRation;
    //cout<<"widthParaTmp is a: "<<widthParaTmp<<endl;

    widthPara = ((float)((int)((widthParaTmp+0.05)*10)))/ 10;
    heighPara = ((float)((int)((heighParaTmp+0.05)*10)))/ 10;

    //cout<<"widthParaTmp is a: "<<widthPara<<endl;
}


statOneRes StatOnePross::doCheckStatusOne(Mat srcMatImg)
{
    statOneRes resTmp;
    resTmp.stationOkFlag = true;

    if (srcMatImg.empty())
    {
        LOGE(TAG, "error, doCheckStatusOne srcImg is empty");
        resTmp.stationOkFlag = false;
        resTmp.errCode.push_back(StationOneNotOpenError);
        return resTmp;
    }

    Mat camMat = mAlgoInfo.camMatrix1;
    Mat distMat = mAlgoInfo.distMatrix1;
    Mat perspMat = mAlgoInfo.perspFromMatrix1;
    Mat unDistPerspImg = getUndistPersp(srcMatImg, camMat, distMat, perspMat); //get the distort and pespective image
    bool openStatusFlag = getOpenStatue(unDistPerspImg);//the box is open
    LOGD(TAG, "a. statOne  openStat is %1", openStatusFlag);
    if (openStatusFlag == true)
    {
        int rectTlx = mAlgoInfo.roiTlX1; int rectTly = mAlgoInfo.roiTlY1;
        int rectWidth = mAlgoInfo.roiWidth1; int rectHight = mAlgoInfo.roiHight1;
        Rect rectROI(rectTlx, rectTly, rectWidth, rectHight);
        Mat srcRoiIMg = getRoiImg(unDistPerspImg, rectROI);//get the roi region of inner rect

        rectInfors rectHW = getRectInfors(srcRoiIMg);//get the inner rect infor
        //pixsConvertLength(rectHW, resTmp.width, resTmp.heigh);
        LOGD(TAG, "b. statOne rectCheck is %1", rectHW.rectOkFlag);
        if(rectHW.rectOkFlag == true)
        {
            resTmp.stationOkFlag = true;
        }
        else
        {
            resTmp.stationOkFlag = false;
            resTmp.errCode.push_back(DimenError);
        }
    }
    else
    {
        resTmp.stationOkFlag = false;
        resTmp.errCode.push_back(StationOneNotOpenError);
    }

    return resTmp;
}
