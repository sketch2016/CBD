#ifndef STORAGESERVICE_H
#define STORAGESERVICE_H

#include "include/defines.h"

#include <QObject>
#include <QThread>
#include <QVector>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>

class CheckStoreTimeOutThread : public QThread {
public:
    CheckStoreTimeOutThread();
    void run();
    void addPath(QString &path);
    void setTimeOut(int timeout);
    void clearPath();

private:
    void checkStoreTime();
    bool deleteDir(QString &path);
    int getStoreTime(QString &path);

private:
    QVector<QString> *paths;
    int timeOut;//days
};

class StorageService : public QObject
{
    Q_OBJECT

public:
    static StorageService* getInstance();
    ~StorageService();
    QString createNewDir(QString name);
    void saveImages(QVector<QImage*>& images, int detect_result, QVector<int>& resultCodes);
    void saveImage(QImage& images, QString &path);
    void addImage(int id, QImage& image);

    QSqlQuery* createSqlConnection();
    void getDetectCount(Detect_Data &detect_Data);
    void updateAndStore(Detect_Data &detect_Data, AnalysisResult *result);
    void applyConfig(Config_Params &params);
    void addPathToThread(QString &path);


private:
    StorageService();
    void init();
    void launchDeleteThread();
    static StorageService *mInstance;
    int storage_strategy;
    int storeTime;
    QString rootPath;
    QSqlQuery *query;
    QHash<int, QImage> images_map;

    CheckStoreTimeOutThread *deleteThread;
    QTimer *timer;
};

#endif // STORAGESERVICE_H
