#include <QMessageBox>
#include <QDir>
#include <QDebug>
#include <QSqlRecord>
#include <omp.h>

#include "include/storage/storageservice.h"
#include "include/util/logger.h"
#include "include/defines.h"

#include <QTimer>

#define TAG "StorageService"
#define ROOTPATH "./pics"
#define DEFECTS_IMGS_DIR "defects_imgs"
#define NODEFECTS_IMGS_DIR "nodefects_imgs"
#define TABLE_CBD "cbd_detect"
#define HOUR 60*60*1000
#define DAY 24*HOUR
#define CHECK_FREQUENCY 4 * HOUR

CheckStoreTimeOutThread::CheckStoreTimeOutThread() {
    paths = new QVector<QString>();
}

void CheckStoreTimeOutThread::run() {
    qDebug()<<"Thread id :" << currentThreadId() << "is running";
    checkStoreTime();
}

void CheckStoreTimeOutThread::addPath(QString &path) {

    this->paths->append(path);
    LOGD(TAG, "CheckStoreTimeOutThread::addPath %1", path);
}

void CheckStoreTimeOutThread::clearPath() {
    if (this->paths != nullptr && this->paths->size() > 0) {
        this->paths->clear();
    }
}

void CheckStoreTimeOutThread::setTimeOut(int timeout) {
    LOGD(TAG, "setTimeOut::timeout = %1", timeout);
    this->timeOut = timeout;
}

void CheckStoreTimeOutThread::checkStoreTime() {

    for (int i = 0; i < paths->size(); i++) {
        QString path = paths->at(i);
        if(path == nullptr || path.size() == 0) {
            LOGD(TAG, "checkStoreTime: invalid path %1", path);
            return;
        }

        QDir dir;
        if (dir.exists(path) && getStoreTime(path) > timeOut) {
            bool res = deleteDir(path);
            LOGD(TAG, "removeDir:%1 result:%2", path, res);
        }
    }
}

bool CheckStoreTimeOutThread::deleteDir(QString &path) {
    if (path.isEmpty()){
        return false;
    }
    QDir dir(path);
    if(!dir.exists()){
        return true;
    }
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); //设置过滤
    QFileInfoList fileList = dir.entryInfoList(); // 获取所有的文件信息
    foreach (QFileInfo file, fileList){ //遍历文件信息
        if (file.isFile()){ // 是文件，删除
            file.dir().remove(file.fileName());
        }
    }
    return dir.rmpath(dir.absolutePath()); // 删除文件夹
}

int CheckStoreTimeOutThread::getStoreTime(QString &name) {
    QStringList names = name.split("/");
    QString path = names.last();
    long start = path.toLong();

    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyyMMdd");
    int current = current_date.toLong();

    long diff = current - start;
    LOGD(TAG, "getStoreTime result = :%1", diff);
    return diff;
}

StorageService *StorageService::mInstance = nullptr;

StorageService::StorageService()
{
    query = createSqlConnection();
    deleteThread = new CheckStoreTimeOutThread();
}

StorageService::~StorageService()
{
    if (query != nullptr) {
        delete query;
        query = nullptr;
    }

    if (deleteThread != nullptr) {
        delete deleteThread;
        deleteThread = nullptr;
    }
}

StorageService *StorageService::getInstance() {
    if(mInstance == nullptr) {
        mInstance = new StorageService();
    }

    return mInstance;
}

void StorageService::init()
{
    if(rootPath == nullptr || rootPath.size() == 0) {
        rootPath = ROOTPATH;
    }
    QDir dir;
    QString dir_str = rootPath;
    if (!dir.exists(dir_str)) {
        bool res = dir.mkpath(dir_str);
        qDebug()<<"creat rootPath result: "<<res;
        if (res) {
            QString res_mk_deteft = createNewDir(DEFECTS_IMGS_DIR);
            qDebug()<<"creat dir_defects result: "<<res_mk_deteft;

            QString res_mk_nodeteft = createNewDir(NODEFECTS_IMGS_DIR);
            qDebug()<<"creat dir_nodefects result: "<<res_mk_nodeteft;

            addPathToThread(res_mk_nodeteft);
        }
    }

    deleteThread->setTimeOut(storeTime);
    timer = new QTimer(this);
    timer->setInterval(CHECK_FREQUENCY);
    connect(timer, &QTimer::timeout, this, &StorageService::launchDeleteThread);
    timer->start();
}

void StorageService::launchDeleteThread() {
    deleteThread->clearPath();

    QDir dir;
    QString dir_defects = rootPath + "/" + DEFECTS_IMGS_DIR;
    if (dir.exists(dir_defects)) {
        addPathToThread(dir_defects);
    }
    QString dir_nodefects = rootPath + "/" + NODEFECTS_IMGS_DIR;
    if (dir.exists(dir_nodefects)) {
        addPathToThread(dir_nodefects);
    }

   qDebug()<<"launchDeleteThread:Thread id :" << this->thread()->currentThreadId() << "is running";
   deleteThread->start();
}

void StorageService::addPathToThread(QString &path)
{
    LOGD(TAG, "addPathToThread: path = %1", path);
    QDir dir(path);
    QStringList files = dir.entryList();
    QRegExp reg("\\d{8}");
    for (QString file : files) {
        if (reg.exactMatch(file)) {
            QString name = path + "/" + file;
            deleteThread->addPath(name);
        }

    }
}

void StorageService::applyConfig(Config_Params &params)
{
    rootPath = params.rootpath;
    this->storage_strategy = params.storeStrategy;
    this->storeTime = params.storeTime;

    init();
}

QString StorageService::createNewDir(QString name) {
    //create dir
    if(rootPath == nullptr || rootPath.size() == 0) {
        return nullptr;
    }

    QDir dir;
    QString dir_str = rootPath + "/" + name;
    if (!dir.exists(dir_str)) {
        bool res = dir.mkpath(dir_str);
        qDebug()<<"creat dir result: "<<res;
    }

    return dir_str;
}

void StorageService::addImage(int id, QImage& image)
{
    QImage img = image;
    images_map.insert(id, img);
}

void StorageService::saveImage(QImage &image, QString &path) {
    LOGD(TAG, "saveImage: path = %1", path);
    image.save(path);
}

void StorageService::saveImages(QVector<QImage*> &images, int detect_result, QVector<int> &resultCodes)
{
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString time = current_date_time.toString("yyyyMMddhhmmss");

    QString dir = rootPath + "/" + (detect_result == Detect_Result::Unqualified ? DEFECTS_IMGS_DIR : NODEFECTS_IMGS_DIR);
    QString path = dir + time + "_defects";

    for (int code : resultCodes) {
        path += "-" + QString::number(code);
    }

    LOGD(TAG, "SaveImages: path = %1", path);

    //image.save(path);

    return;
}

QSqlQuery* StorageService::createSqlConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setPort(3306);
    db.setDatabaseName("cbd");
    //这里输入你的数据库名
    db.setUserName("root");
    db.setPassword("123456"); //这里输入你的密码
    if (!db.open()) {
        QMessageBox::critical(0, QObject::tr("无法打开数据库"), "无法创建数据库连接！ ", QMessageBox::Cancel);
        return nullptr;
    }

    QSqlQuery *query = new QSqlQuery(db);
    QStringList tables = db.tables();
    if (!tables.contains(TABLE_CBD)) {
        // 创建 cbd 表
        query->exec("create table if not exists cbd_detect (id int primary key, total_count int, unqualified_count int, date varchar(20))");

        QDateTime current_date_time = QDateTime::currentDateTime();
        QString time = current_date_time.toString("yyyyMMdd-hh:mm:ss");
        QString sql = QString("insert into cbd_detect values(%1, %2, %3, '%4');")
                    .arg(1)
                    .arg(0)
                    .arg(0)
                    .arg(time);

        LOGD(TAG, "sql = %1", sql);
        query->exec(sql);
    }

    return query;

}

void StorageService::getDetectCount(Detect_Data &detect_Data)
{
    if (query == nullptr) {
        return;
    }

    query->exec("select * from cbd_detect");
    while(query->next()) {
        detect_Data.total = query->value(1).toInt();
        detect_Data.unqualified = query->value(2).toInt();
        detect_Data.time = query->value(3).toString();
    }

    QString out = QString("getDetectCount(total:%1, unqualified:%2, time:%3)")
                .arg(detect_Data.total)
                .arg(detect_Data.unqualified)
                .arg(detect_Data.time);
    LOGD(TAG, out);
}

void StorageService::updateAndStore(Detect_Data &add_Data, AnalysisResult *result)
{
    Detect_Data pre_data = {0, 0, ""};
    getDetectCount(pre_data);

    QDateTime current_date_time = QDateTime::currentDateTime();
    QString time = current_date_time.toString("yyyyMMdd-hh:mm:ss");

    QString sql = QString("update cbd_detect set total_count = %1, unqualified_count = %2, date = '%3' where id = %4;")
                .arg(add_Data.total + pre_data.total)
                .arg(add_Data.unqualified + pre_data.unqualified)
                .arg(time)
                .arg(1);

    LOGD(TAG, "sql = %1", sql);
    if (query->exec(sql)) {
        LOGD(TAG, "updateAndStore:update database success");
    }

    int detectResult = result->actual_result;
    if (this->storage_strategy == 0 && detectResult == Detect_Result::Qualified) {
        LOGD(TAG, "updateAndStore:only store unqualified pic, return");
        return;
    }

    //store images
    #pragma omp parallel for
    for (oneFrameResult ofr : result->results) {
        int id = ofr.camera_id;
        QImage img = images_map.value(id);
        time = current_date_time.toString("yyyyMMdd");

        QString dir = rootPath + "/" + (result->actual_result == Detect_Result::Unqualified ? DEFECTS_IMGS_DIR : NODEFECTS_IMGS_DIR) + "/";
        QString dateDir = dir + time;
        QDir qdir;
        if (!qdir.exists(dateDir)) {
            bool res = qdir.mkpath(dateDir);
            qDebug()<<"creat dir result: "<<res;
        }

        time = current_date_time.toString("yyyyMMddhhmmss");
        QString path = dateDir + "/" + time +  "_defects";

        for (int code : ofr.error_codes) {
            path += "-" + QString::number(code);
        }

        path += ".bmp";
        LOGD(TAG, "SaveImage: path = %1", path);
        if (img.save(path)) {
            LOGD(TAG, "updateAndStore:save image success");
        }

    }

    images_map.clear();

}
