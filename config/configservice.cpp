#include "include/config/configservice.h"
#include "include/util/logger.h"
#include "include/util/autolock.h"

#include <QFile>
#include <QTextStream>
#include <QString>
#include <QSettings>
#include <QDir>

#define FILE_CONFIG "./config.ini"
#define TAG_CONFIG "ConfigService"

#define STORAGE_STRATEGY "store_strategy"
#define STORE_TIME "store_time"
#define DEBUGMODE "debug"
#define PLC_ADDR "plc_addr"
#define PLC_SERVER_ADDR "plc_server_addr"
#define DISPLAY_ADDR "diaplay_addr"
#define ROOTPATH "rootpath"
#define DEFAULT_ROOTPATH "/home/sketchxu/qt_projects/CBD/pics"
#define STATION "station"
#define SERIAL_NUMBER "serial_number"
#define ILLUMINATE_NUMBER "illuminant_number"
#define OPTOELECTRO_NUMBER "optoelectro_number"
#define ILLUMCONTROLLER_NUMBER "illumController_number"
#define SERIAL_PORT "serialport"
#define WIDTH "image_width"
#define HEIGHT "image_height"

ConfigService *ConfigService::mInstance = nullptr;
QMutex ConfigService::mInstanceMutex;

ConfigService::ConfigService()
{
    //writeConfigToFile();
    //readConfigFromFile(params);
}

ConfigService *ConfigService::getInstance()
{
    //AutoLock lock(mInstanceMutex);
    if (!mInstance) {
        mInstance = new ConfigService();
    }

    return mInstance;
}

ConfigService::~ConfigService()
{

}

void ConfigService::onDebugModeChanged(bool debugMode)
{
    QString debug = QString::number(debugMode);
    if (debugMode == params.debug) {
        LOGD(TAG_CONFIG, "debugMode no change, return");
    }

    writeConfigToFile(DEBUGMODE, debug);
    params.debug = debugMode;
}


void ConfigService::onStoreStrategyChanged(int strategy)
{
    QString storage_strategy = QString::number(strategy);
    if (storage_strategy == params.storeStrategy) {
        LOGD(TAG_CONFIG, "storage_strategy no change, return");
    }

    writeConfigToFile(STORAGE_STRATEGY, storage_strategy);
    params.storeStrategy = strategy;
}

void ConfigService::onPlcAddrChanged(QString &newAddr)
{
    if (newAddr == params.plcAddr) {
        LOGD(TAG_CONFIG, "plcAddr no change, return");
    }

    writeConfigToFile(PLC_ADDR, newAddr);
    params.plcAddr = newAddr;
}

void ConfigService::onDisplayAddrChanged(QString &newAddr)
{
    if (newAddr == params.displayAddr) {
        LOGD(TAG_CONFIG, "displayAddr no change, return");
    }

    writeConfigToFile(DISPLAY_ADDR, newAddr);
    params.plcAddr = newAddr;
}

void ConfigService::onRootpathChanged(QString &newPath)
{
    if (newPath == params.rootpath) {
        LOGD(TAG_CONFIG, "rootpath no change, return");
        return;
    }

    QDir dir;
    if (!dir.exists(newPath)) {
        LOGD(TAG_CONFIG, "invalid rootpath, return");
        return;
    }

    writeConfigToFile(ROOTPATH, newPath);
}

void ConfigService::onPlcServerAddrChanged(QString &newPlcServerAddr)
{
    if (newPlcServerAddr == params.plcServerAddr) {
        LOGD(TAG_CONFIG, "PlcServerAddr no change, return");
    }

    writeConfigToFile(PLC_SERVER_ADDR, newPlcServerAddr);
    params.plcServerAddr = newPlcServerAddr;
}

void ConfigService::onStoreTimeChanged(QString &newStoreTime)
{
    if (newStoreTime > 180 || newStoreTime <= 0) {
        LOGD(TAG_CONFIG, "invalid StoreTime: %1", newStoreTime);
        return;
    }

    if (newStoreTime == params.storeTime) {
        LOGD(TAG_CONFIG, "StoreTime no change, return");
    }

    writeConfigToFile(STORE_TIME, newStoreTime);
    params.storeTime = newStoreTime.toInt();
}

void ConfigService::readConfigFromFile(Config_Params &config_params)
{
    LOGD(TAG_CONFIG, "readConfigFromFile");

    QSettings *configIniRead = new QSettings(FILE_CONFIG, QSettings::IniFormat);
    //将读取到的ini文件保存在QString中，先取值，然后通过toString()函数转换成QString类型
    config_params.displayAddr = configIniRead->value(DISPLAY_ADDR).toString();
    config_params.plcAddr = configIniRead->value(PLC_ADDR).toString();
    config_params.plcServerAddr = configIniRead->value(PLC_SERVER_ADDR).toString();
    config_params.debug = configIniRead->value(DEBUGMODE).toBool();
    config_params.storeStrategy = configIniRead->value(STORAGE_STRATEGY).toInt();
    config_params.storeTime = configIniRead->value(STORE_TIME).toInt();
    config_params.rootpath = configIniRead->value(ROOTPATH).toString();
    config_params.station_info.station_id = configIniRead->value(STATION).toInt();
    config_params.station_info.illuminant_num = configIniRead->value(ILLUMINATE_NUMBER).toInt();
    config_params.station_info.optoelectronic_num = configIniRead->value(OPTOELECTRO_NUMBER).toInt();
    config_params.station_info.illumController_num = configIniRead->value(ILLUMCONTROLLER_NUMBER).toInt();
    config_params.station_info.image_width = configIniRead->value(WIDTH).toInt();
    config_params.station_info.image_height = configIniRead->value(HEIGHT).toInt();

    QString serial_numbers = configIniRead->value(SERIAL_NUMBER).toString();
    QStringList numbers = serial_numbers.split("-");
    QString s_numbers;
    for (QString num_tmp : numbers) {
            QStringList id_and_nums = num_tmp.split(":");
            config_params.station_info.camera_serial_num.insert(id_and_nums.at(0).toInt(), id_and_nums.at(1));
            s_numbers.append(id_and_nums.at(0)).append(":").append(id_and_nums.at(1)).append("-");
    }

    QString serial_ports = configIniRead->value(SERIAL_PORT).toString();
    QStringList port_nums = serial_ports.split("-");
    QString p_nums;
    for (QString num : port_nums) {
            QStringList id_and_nums = num.split(":");
            config_params.station_info.serial_port.insert(id_and_nums.at(0).toInt(), id_and_nums.at(1));
            p_nums.append(id_and_nums.at(0)).append(":").append(id_and_nums.at(1)).append("-");
    }

    QString out_str = QString("displayAddr:%1 plcAddr:%2 plcServerAddr:%11 debug:%3 storeStrategy:%4 "
                              "rootpath:%5 station:%6 illuminant_number:%7 optoelectro_number:%8 serial_number:%9 serial_port:%10")
                .arg(config_params.displayAddr)
                .arg(config_params.plcAddr)
                .arg(config_params.debug)
                .arg(config_params.storeStrategy)
                .arg(config_params.rootpath)
                .arg(config_params.station_info.station_id)
                .arg(config_params.station_info.illuminant_num)
                .arg(config_params.station_info.optoelectronic_num)
                .arg(s_numbers)
                .arg(p_nums)
                .arg(config_params.plcServerAddr);
    //打印得到的结果
    LOGD(TAG_CONFIG, "readConfigFromFile:%1", out_str);

    params = config_params;
    //读入入完成后删除指针
    delete configIniRead;
    configIniRead = nullptr;
}

void ConfigService::readAlgoConfigFromFile(Config_Params &config_params)
{
    QSettings *algoConfigIniRead = new QSettings(FILE_CONFIG, QSettings::IniFormat);
    //将读取到的ini文件保存在QString中，先取值，然后通过toString()函数转换成QString类型
    config_params.algorithm_info.pixBaseRation1 = algoConfigIniRead->value("pixBaseRation/baseRation1").toFloat();
    config_params.algorithm_info.pixBaseRation2 = algoConfigIniRead->value("pixBaseRation/baseRation2").toFloat();

    // check open status config  grayValue and openRation
    config_params.algorithm_info.openGrayThresh1 = algoConfigIniRead->value("openInfor/thresh1").toInt();
    config_params.algorithm_info.openFillRation1 = algoConfigIniRead->value("openInfor/fillRation1").toFloat();
    config_params.algorithm_info.openGrayThresh2 = algoConfigIniRead->value("openInfor/thresh2").toInt();
    config_params.algorithm_info.openFillRation2 = algoConfigIniRead->value("openInfor/fillRation2").toFloat();

    // the overLap check config
    config_params.algorithm_info.lapLengthScale = algoConfigIniRead->value("overLapInfor/lengthScale").toFloat();
    config_params.algorithm_info.lapAreaScale = algoConfigIniRead->value("overLapInfor/areaScale").toFloat();

    // get the inner rect config,  the rect position
    config_params.algorithm_info.roiTlX1 = algoConfigIniRead->value("roiInfor/roiTlx1").toInt();
    config_params.algorithm_info.roiTlY1 = algoConfigIniRead->value("roiInfor/roiTly1").toInt();
    config_params.algorithm_info.roiWidth1 = algoConfigIniRead->value("roiInfor/roiWidth1").toInt();
    config_params.algorithm_info.roiHight1 = algoConfigIniRead->value("roiInfor/roiHight1").toInt();
    config_params.algorithm_info.roiTlX2 = algoConfigIniRead->value("roiInfor/roiTlx2").toInt();
    config_params.algorithm_info.roiTlY2 = algoConfigIniRead->value("roiInfor/roiTly2").toInt();
    config_params.algorithm_info.roiWidth2 = algoConfigIniRead->value("roiInfor/roiWidth2").toInt();
    config_params.algorithm_info.roiHight2 = algoConfigIniRead->value("roiInfor/roiHight2").toInt();

    //the statusOne radius rect cofig, the width and height
    config_params.algorithm_info.rectGrayThresh1 = algoConfigIniRead->value("rectInfor/grayThresh1").toInt();
    config_params.algorithm_info.rectWidth = algoConfigIniRead->value("rectInfor/width").toInt();
    config_params.algorithm_info.rectHight = algoConfigIniRead->value("rectInfor/hight").toInt();
    config_params.algorithm_info.rectDeltScale = algoConfigIniRead->value("rectInfor/deltaScale").toFloat();

    //the statusTwo gzj and hgz config
    config_params.algorithm_info.whiteh = algoConfigIniRead->value("hgzGzjInfor/whiteh").toInt();
    config_params.algorithm_info.whiteH = algoConfigIniRead->value("hgzGzjInfor/whiteH").toInt();
    config_params.algorithm_info.whites = algoConfigIniRead->value("hgzGzjInfor/whites").toInt();
    config_params.algorithm_info.whiteS = algoConfigIniRead->value("hgzGzjInfor/whiteS").toInt();
    config_params.algorithm_info.whitev = algoConfigIniRead->value("hgzGzjInfor/whitev").toInt();
    config_params.algorithm_info.whiteV = algoConfigIniRead->value("hgzGzjInfor/whiteV").toInt();

    //the statusTwo gzj config, the hsv and ration
    config_params.algorithm_info.gzjWidth = algoConfigIniRead->value("gzjInfor/width").toInt();
    config_params.algorithm_info.gzjHight = algoConfigIniRead->value("gzjInfor/hight").toInt();

    //the statusTwo hgz config, the black and red hsv value
    config_params.algorithm_info.hgzWidth = algoConfigIniRead->value("hgzInfor/width").toInt();
    config_params.algorithm_info.hgzHight = algoConfigIniRead->value("hgzInfor/hight").toInt();
    config_params.algorithm_info.hgzRedh = algoConfigIniRead->value("hgzInfor/redh").toInt();
    config_params.algorithm_info.hgzRedH = algoConfigIniRead->value("hgzInfor/redH").toInt();
    config_params.algorithm_info.hgzReds = algoConfigIniRead->value("hgzInfor/reds").toInt();
    config_params.algorithm_info.hgzRedS = algoConfigIniRead->value("hgzInfor/redS").toInt();
    config_params.algorithm_info.hgzRedv = algoConfigIniRead->value("hgzInfor/redv").toInt();
    config_params.algorithm_info.hgzRedV = algoConfigIniRead->value("hgzInfor/redV").toInt();
    config_params.algorithm_info.hgzBlackb = algoConfigIniRead->value("hgzInfor/blackb").toInt();
    config_params.algorithm_info.hgzBlackB = algoConfigIniRead->value("hgzInfor/blackB").toInt();
    config_params.algorithm_info.hgzBlackg = algoConfigIniRead->value("hgzInfor/blackg").toInt();
    config_params.algorithm_info.hgzBlackG = algoConfigIniRead->value("hgzInfor/blackG").toInt();
    config_params.algorithm_info.hgzBlackr = algoConfigIniRead->value("hgzInfor/blackr").toInt();
    config_params.algorithm_info.hgzBlackR = algoConfigIniRead->value("hgzInfor/blackR").toInt();
    config_params.algorithm_info.hgzRedRation = algoConfigIniRead->value("hgzInfor/redRation").toFloat();
    config_params.algorithm_info.hgzBlackRation = algoConfigIniRead->value("hgzInfor/blackRation").toFloat();

    //the statusTwo sd config
    config_params.algorithm_info.sdGrayThresh = algoConfigIniRead->value("sdInfor/thresh").toInt();
    config_params.algorithm_info.sdRoiTlX = algoConfigIniRead->value("sdInfor/tlx").toInt();
    config_params.algorithm_info.sdRoiTlY = algoConfigIniRead->value("sdInfor/tly").toInt();
    config_params.algorithm_info.sdRoiWidth = algoConfigIniRead->value("sdInfor/roiWidth").toInt();
    config_params.algorithm_info.sdRoiHight = algoConfigIniRead->value("sdInfor/roiHight").toInt();
    config_params.algorithm_info.sdLength = algoConfigIniRead->value("sdInfor/length").toInt();
    config_params.algorithm_info.sdBgLength = algoConfigIniRead->value("sdInfor/sdBgLength").toInt();
    config_params.algorithm_info.sds = algoConfigIniRead->value("sdInfor/sds").toInt();
    config_params.algorithm_info.sdS = algoConfigIniRead->value("sdInfor/sdS").toInt();
    config_params.algorithm_info.sdh = algoConfigIniRead->value("sdInfor/sdh").toInt();
    config_params.algorithm_info.sdH = algoConfigIniRead->value("sdInfor/sdH").toInt();
    config_params.algorithm_info.sdv = algoConfigIniRead->value("sdInfor/sdv").toInt();
    config_params.algorithm_info.sdV= algoConfigIniRead->value("sdInfor/sdV").toInt();

    //debug save the process image ,
    config_params.algorithm_info.saveSation1 = algoConfigIniRead->value("saveOKCheck/station1").toInt();
    config_params.algorithm_info.saveSation2 = algoConfigIniRead->value("saveOKCheck/station2").toInt();

    config_params.algorithm_info.checkExitFlag = algoConfigIniRead->value("testExitFlag/flag").toInt();

    params = config_params;
    //读入入完成后删除指针
    delete algoConfigIniRead;
    algoConfigIniRead = nullptr;
}

void ConfigService::readDistortParamFromFile(Config_Params &config_params)
{
    QSettings *distortParamIniRead = new QSettings(FILE_CONFIG, QSettings::IniFormat);

    QString camMtxOnePath = distortParamIniRead->value("distortParamPath/cammtxonepath").toString();
    cv::FileStorage fsCam(camMtxOnePath.toStdString(), cv::FileStorage::READ);
    if (!fsCam.isOpened())
    {
      LOGE(TAG_CONFIG, "failed to open %1", camMtxOnePath);
      return;
    }
    fsCam["cameraMatrix"]>>config_params.algorithm_info.camMatrix1;
    fsCam.release();

    QString distMtxOnePath = distortParamIniRead->value("distortParamPath/distmtxonepath").toString();
    cv::FileStorage fsDis(distMtxOnePath.toStdString(), cv::FileStorage::READ);
    if (!fsDis.isOpened())
    {
      LOGE(TAG_CONFIG, "failed to open 1%", distMtxOnePath);
      return;
    }
    fsDis["distCoeffs"]>>config_params.algorithm_info.distMatrix1;
    fsDis.release();

    QString perspMtxOnePath = distortParamIniRead->value("distortParamPath/perspmtxonepath").toString();
    cv::FileStorage fsPersp(perspMtxOnePath.toStdString(), cv::FileStorage::READ);
    if (!fsPersp.isOpened())
    {
      LOGE(TAG_CONFIG, "failed to open 1%", perspMtxOnePath);
      return;
    }
    fsPersp["perspMat"]>>config_params.algorithm_info.perspFromMatrix1;
    fsPersp.release();

    // the camera2 paramera
    QString camMtxTwoPath = distortParamIniRead->value("distortParamPath/cammtxtwopath").toString();
    cv::FileStorage fsCam2(camMtxTwoPath.toStdString(), cv::FileStorage::READ);
    if (!fsCam2.isOpened())
    {
      LOGE(TAG_CONFIG, "failed to open 1%", camMtxTwoPath);
      return;
    }
    fsCam2["cameraMatrix"]>>config_params.algorithm_info.camMatrix2;
    fsCam2.release();

    QString distMtxTwoPath = distortParamIniRead->value("distortParamPath/distmtxtwopath").toString();
    cv::FileStorage fsDis2(distMtxTwoPath.toStdString(), cv::FileStorage::READ);
    if (!fsDis2.isOpened())
    {
      LOGE(TAG_CONFIG, "failed to open 1%", distMtxTwoPath);
      return;
    }
    fsDis2["distCoeffs"]>>config_params.algorithm_info.distMatrix2;
    fsDis2.release();

    QString perspMtxTwoPath = distortParamIniRead->value("distortParamPath/perspmtxtwopath").toString();
    cv::FileStorage fsPersp2(perspMtxTwoPath.toStdString(), cv::FileStorage::READ);
    if (!fsPersp2.isOpened())
    {
      LOGE(TAG_CONFIG, "failed to open 1%", perspMtxTwoPath);
      return;
    }
    fsPersp2["perspMat"]>>config_params.algorithm_info.perspFromMatrix2;
    fsPersp2.release();

    params = config_params;
    //读入入完成后删除指针
    delete distortParamIniRead;
    distortParamIniRead = nullptr;
}

void ConfigService::writeConfigToFile(Config_Params &config_params)
{
    LOGD(TAG_CONFIG, "writeConfigToFile:%1");
    //Qt中使用QSettings类读写ini文件
    //QSettings构造函数的第一个参数是ini文件的路径,第二个参数表示针对ini文件,第三个参数可以缺省
    QSettings *configIniWrite = new QSettings(FILE_CONFIG, QSettings::IniFormat);
    //向ini文件中写入内容,setValue函数的两个参数是键值对
    //向ini文件的第一个节写入内容,ip节下的第一个参数
    configIniWrite->setValue(DISPLAY_ADDR, "192.168.8.127:8080");
    //向ini文件的第一个节写入内容,ip节下的第二个参数
    configIniWrite->setValue(PLC_ADDR, "192.168.8.127:9099");
    configIniWrite->setValue(PLC_SERVER_ADDR, "192.168.8.127:8888");

    configIniWrite->setValue(DEBUGMODE, "1");
    configIniWrite->setValue(STORAGE_STRATEGY, "0");
    configIniWrite->setValue(STORE_TIME, "180");
    configIniWrite->setValue(ROOTPATH, DEFAULT_ROOTPATH);
    configIniWrite->setValue(STATION, "1");
    configIniWrite->setValue(SERIAL_NUMBER, "2J005B8PAK00115");
    configIniWrite->setValue(ILLUMINATE_NUMBER, "1");
    configIniWrite->setValue(OPTOELECTRO_NUMBER, "1");
    //写入完成后删除指针

    delete configIniWrite;
}

void ConfigService::writeConfigToFile(QString key, QString value)
{
    LOGD(TAG_CONFIG, "key=%1, value=%2", key, value);
    QSettings *configIniWrite = new QSettings(FILE_CONFIG, QSettings::IniFormat);
    configIniWrite->setValue(key, value);
    //写入完成后删除指针
    delete configIniWrite;
}
