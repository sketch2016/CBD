#include "include/util/logger.h"
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QTextStream>

#define LOG_TAG_BLCAKLIST_CONFIG "./logblacklist.ini"

int Log::loglevel = LOG_DEBUG;
FileWatchThread *Log::mFileWatchThread = nullptr;
QVector<QString> Log::mBlackList;
QMutex Log::mInitMutex;
int Log::isInited = LOG_IDLE;

#define TAG "LOGGER"

void FileWatchThread::run(){
    this->mSystemWatcher = new QFileSystemWatcher();
    this->mSystemWatcher->addPath(LOG_TAG_BLCAKLIST_CONFIG);
    connect(mSystemWatcher, SIGNAL(fileChanged(QString)), this, SLOT(onFileUpdated(QString)));
    //LOGD(TAG,"FileWatchThread run");
    exec();
}

void FileWatchThread::onFileUpdated(QString file) {
    //LOGD(TAG,"FileWatchThread onFileUpdate");
    if(file.compare(LOG_TAG_BLCAKLIST_CONFIG) == 0) {
        Log::loadConfig();
        disconnect(mSystemWatcher, SIGNAL(fileChanged(QString)), this, SLOT(onFileUpdated(QString)));
        mSystemWatcher->removePath(LOG_TAG_BLCAKLIST_CONFIG);
        mSystemWatcher->addPath(LOG_TAG_BLCAKLIST_CONFIG);
        connect(mSystemWatcher, SIGNAL(fileChanged(QString)), this, SLOT(onFileUpdated(QString)));
    }
}

void Log::start() {
    mInitMutex.lock();
    if(isInited == LOG_IDLE) {
        loadConfig();
        mFileWatchThread = new FileWatchThread();
        mFileWatchThread->start();
    }

    isInited = LOG_INITED;
    mInitMutex.unlock();
}

void Log::loadConfig() {
    //LOGD(TAG,"loadConfig!!!!");
    QFile file(LOG_TAG_BLCAKLIST_CONFIG);
    if(!file.exists()) {
        return;
    }

    file.open(QFile::ReadOnly);
    QVector<QString> blacktags;

    QTextStream in(&file);
    QString line;
    while((line = in.readLine())!= nullptr) {
        if(line.contains("#")) {
            continue;
        }
        //qDebug()<<"tag is "<<line;
        QString tag = line.simplified();
        blacktags.append(tag);
    }

    mBlackList.clear();
    mBlackList.swap(blacktags);
}
