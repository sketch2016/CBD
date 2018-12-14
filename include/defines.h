#ifndef DEFINES_H
#define DEFINES_H

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <QString>
#include <QVector>
#include <QMap>
#include "include/hardware/cframeinfo.h"

using namespace cv;

#define IPCONFIG_PATH "./config_file/ip_config.txt"
#define DETECTORCONFIG_PATH "./config_file/detectorconfig.prototxt"

#define CAMERA_HEAT_WIRED_PORT 8699
#define CAMERA_ERROR_FRAME_MAX 32

#define CAMERA_DISCONNECT_WAIT_TIME 1000

#define EXTERNAL(X)
#define INTERNAL(X)

struct point
{
    int x;
    int y;
};

struct defect_area
{
    point p;
    int width;
    int height;
};

//result for each frame
struct oneFrameResult
{
    int camera_id;
    int frame_result;
    QVector<int> error_codes;
    QVector<defect_area> defect_areas;
};

struct AnalysisResult
{
    int actual_result;
    QVector<oneFrameResult> results;
};

typedef struct stationInformation {
    int station_id;
    QMap<int,QString> camera_serial_num;
    int illuminant_num;
    int illumController_num;
    int optoelectronic_num;
    int image_width;
    int image_height;
    QMap<int,QString> serial_port;
} STATIONINFO;

typedef struct algorithmInformation {
    float pixBaseRation1;
    float pixBaseRation2;

    int openGrayThresh1;
    float openFillRation1;
    int openGrayThresh2;
    float openFillRation2;

    float lapLengthScale, lapAreaScale;

    int roiTlX1, roiTlY1;
    int roiWidth1, roiHight1;
    int roiTlX2, roiTlY2;
    int roiWidth2, roiHight2;

    int rectGrayThresh1;
    int rectWidth;
    int rectHight;
    float rectDeltScale;

    int whiteh, whites, whiteS;
    int whiteH, whitev, whiteV;

    int gzjWidth, gzjHight;

    int hgzWidth, hgzHight;
    int hgzRedh, hgzRedH, hgzReds;
    int hgzRedS, hgzRedv, hgzRedV;
    int hgzBlackb, hgzBlackB, hgzBlackg;
    int hgzBlackG, hgzBlackr, hgzBlackR;
    float hgzRedRation, hgzBlackRation;

    int sdGrayThresh;
    int sdRoiTlX, sdRoiTlY;
    int sdRoiWidth, sdRoiHight;
    int sdLength, sdBgLength;
    int sds,sdS, sdh, sdH, sdv, sdV;

    int saveSation1;
    int saveSation2;

    Mat camMatrix1, camMatrix2;
    Mat distMatrix1, distMatrix2;
    Mat perspFromMatrix1, perspFromMatrix2;

    int checkExitFlag;
} ALGOINFO;

struct Config_Params
{
    Config_Params(){}
    bool debug;
    int storeStrategy;
    QString plcAddr;
    QString plcServerAddr;
    QString displayAddr;
    QString rootpath;
    int storeTime;

    STATIONINFO station_info;
    ALGOINFO algorithm_info;
};

struct Detect_Data
{
    int total;
    int unqualified;
    QString time;
};

enum Station {
    Station_One,
    Station_Two,
    Station_Three
};
enum Error_Type {
    NoError,
    StationOneNotOpenError,
    //NoDryerError,
    DimenError,
    StationTwoNotOpenError,
    //DryerStillError,
    CertificateError,
    SilkError
};
enum Detect_Result {
    Unqualified,
    Qualified
};
enum Machine_State {
    Running,
    Stopped,
    Warning,
    Reparing
};

#endif // DEFINES_H

