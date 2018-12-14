#ifndef CONFIGSERVICE_H
#define CONFIGSERVICE_H

#include <QMutex>
#include <QObject>

#include "include/ui/dialog.h"
#include "include/defines.h"

class ConfigService : public ConfigChangeListener
{

public:
//    struct Config_Params
//    {
//        Config_Params(){}
//        bool debug;
//        int storeStrategy;
//        QString plcAddr;
//        QString displayAddr;
//        QString rootpath;

//    };
    static ConfigService* getInstance();
    ~ConfigService();
    void readConfigFromFile(Config_Params &config_params);
    void readAlgoConfigFromFile(Config_Params &config_params);
    void readDistortParamFromFile(Config_Params &config_params);
    void writeConfigToFile(Config_Params &config_params);
    void writeConfigToFile(QString key, QString value);

    //ConfigChangeListener
    void onDebugModeChanged(bool debug);
    void onStoreStrategyChanged(int newStrategy);
    void onPlcAddrChanged(QString &newPlcAddr);
    void onDisplayAddrChanged(QString &newDisplayAddr);
    void onRootpathChanged(QString &newPath);
    void onPlcServerAddrChanged(QString &newPlcServerAddr);
    void onStoreTimeChanged(QString &newStoreTime);


private:
    ConfigService();
    static ConfigService *mInstance;
    static QMutex mInstanceMutex;
    Config_Params params;
};

#endif // CONFIGSERVICE_H
