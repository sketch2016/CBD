#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QDebug>
#include <QDateTime>
#include <QMutex>
#include <QtGlobal>
#include <QPixmap>
#include <QVector>
#include <QFileSystemWatcher>
#include <QThread>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

enum LOG_LEVEL {
    LOG_DEBUG = 0,
    LOG_INFO,
    LOG_WARNIG,
    LOG_ERROR
};

enum LOG_STATUS {
    LOG_IDLE = 0,
    LOG_INITED
};


class FileWatchThread:public QThread {

    Q_OBJECT

private:
    QFileSystemWatcher *mSystemWatcher;

private slots:
    void onFileUpdated(QString);

public:
    void run();

};

#define LOGD Log::d
#define LOGI Log::i
#define LOGW Log::w
#define LOGE Log::e

class Log {
public:
    //static void start();

    template <typename... Types>
    static inline void e(QString tag,QString log,const Types&...args) {
        if(loglevel <= LOG_ERROR) {
            logger_print(tag,log,args...);
        }
    }

    template <typename... Types>
    static inline void d(QString tag,QString log,const Types&...args) {
        if(loglevel <= LOG_DEBUG) {
            logger_print(tag,log,args...);
        }
    }

    template <typename... Types>
    static inline void w(QString tag,QString log,const Types&...args) {
        if(loglevel <= LOG_WARNIG) {
            logger_print(tag,log,args...);
        }
    }

    template <typename... Types>
    static inline void i(QString tag,QString log,const Types&...args) {
        if(loglevel <= LOG_INFO) {
            logger_print(tag,log,args...);
        }
    }

    static inline void e(QString tag,QString log) {
        if(loglevel <= LOG_ERROR) {
            logger_print(tag,log);
        }
    }

    static inline void d(QString tag,QString log) {
        if(loglevel <= LOG_DEBUG) {
            logger_print(tag,log);
        }
    }

    static inline void w(QString tag,QString log) {
        if(loglevel <= LOG_WARNIG) {
            logger_print(tag,log);
        }
    }

    static inline void i(QString tag,QString log) {
        if(loglevel >= LOG_INFO) {
            logger_print(tag,log);
        }
    }


    static inline void updateLevel(int level) {
          loglevel = level;
    }

    static void loadConfig();

private:
    static QVector<QString> mBlackList;
    static FileWatchThread *mFileWatchThread;
    static QMutex mInitMutex;
    static int isInited;

    static void start();

    template <typename... Types>
    static void logger_print(QString tag,QString log,const Types&...args) {
        if(isInited == LOG_IDLE) {
            start();
        }

        //check tag
#if 0
        int size = mBlackList.size();
        for(int i = 0;i<size;i++) {
            QString blacktag = mBlackList.at(i);
            if(blacktag.compare(tag) == 0) {
                return;
            }
        }
#endif
        if(mBlackList.indexOf(tag) >= 0) {
            return;
        }

        QDateTime current_date_time = QDateTime::currentDateTime();
        QString time = current_date_time.toString("yyyy/MM/dd hh:mm:ss.zzz");
        QString out_str = QString("%1 :[%2] %3")
                .arg(time)
                .arg(tag)
                .arg(log);

        QString result = logprinter_loop(out_str,args...);
        qCritical()<<qPrintable(result);
    }

    template <typename T,typename... Types>
    static QString logprinter_loop(QString log,T t,Types...args) {
        QString val = toString(t);
        QString new_log = log.arg(val);

        return logprinter_loop(new_log,args...);
    }

    static inline QString logprinter_loop(QString log) {
        //all the log has been analysed
        return log;
    }

    static inline void logger_print(QString tag,QString log) {

        //check tag
        if(isInited == LOG_IDLE) {
            start();
        }

#if 0
        int size = mBlackList.size();
        for(int i = 0;i<size;i++) {
            QString blacktag = mBlackList.at(i);
            if(blacktag.compare(tag) == 0) {
                return;
            }
        }
#endif
        if(mBlackList.indexOf(tag) > 0) {
            return;
        }

        //we should change char to QString
        QDateTime current_date_time = QDateTime::currentDateTime();
        QString time = current_date_time.toString("yyyy/MM/dd hh:mm:ss.zzz");
        QString out_str = QString("%1 :[%2] %3")
                    .arg(time)
                    .arg(tag)
                    .arg(log);

        qCritical()<<qPrintable(out_str);
    }

    inline static QString toString(QChar t) {
        return QString(t);
    }

    inline static QString toString(bool t) {
        if(t) {
            return "true";
        } else {
            return "false";
        }
    }

    inline static QString toString(char t) {
        return QString(t);
    }

    inline static QString toString(signed short t) {
        return QString::number(t);
    }

    inline static QString toString(unsigned short t) {
        return QString::number(t);
    }

    inline static QString toString(signed int t) {
        return QString::number(t);
    }

    inline static QString toString(unsigned int t) {
        return QString::number(t);
    }

    inline static QString toString(signed long t) {
        return QString::number(t);
    }

    inline static QString toString(unsigned long t) {
        return QString::number(t);
    }

    inline static QString toString(qint64 t) {
        return QString::number(t);
    }

    inline static QString toString(quint64 t) {
        return QString::number(t);
    }

    inline static QString toString(float t) {
        return QString::number(t);
    }

    inline static QString toString(double t) {
        return QString::number(t);
    }

    inline static QString toString(const char* t) {
        return QString(t);
    }

    inline static QString toString(const QString & t) {
        return QString(t);
    }

    inline static QString toString(QLatin1String t) {
        return QString(t);
    }

    inline static QString toString(const QByteArray & t) {
        return QString(t);
    }

    inline static QString toString(const QFile &file) {
        QString filestr = QString("file path is %1,size is %2")
                .arg(file.fileName()).arg(file.size());

        return filestr;
    }

    inline static QString toString(QSize &t) {
        int width = t.width();
        int height = t.height();
        QString ret = QString("QSize(width:%1,height:%2)").arg(width).arg(height);
        return ret;
    }

    inline static QString toString(const QJsonObject obj) {
        QString start = "QJsonObject[";
        QStringList keys = obj.keys();
        int length = keys.size();
        for(int i = 0;i<length;i++) {
            QString key = keys.at(i);
            QJsonValue val = obj.value(key);
            start += "(" + key + ":";

            QString group = toString(val);
            start += group;
            start += ")";
        }
        start += "]";
        return start;
    }

    inline static QString toString(const QJsonValue jsonval) {
        QString start="";
        if(jsonval.isArray()) {
            QJsonArray array = jsonval.toArray();
            int size = array.size();
            start += "Array[";
            for(int i = 0;i < size;i++) {
                QJsonValue val = array.at(i);
                QString str = toString(val);
                start += str;
                if(i < size - 1) {
                    start += ",";
                }
            }
            start += "]";
        } else {
            if(jsonval.type() == QJsonValue::Bool) {
                bool val = jsonval.toBool();
                start += QString(val);
            } else if(jsonval.type() == QJsonValue::Double) {
                double val = jsonval.toDouble();
                start += QString::number(val);
            } else if(jsonval.type() == QJsonValue::String) {
                start += jsonval.toString();
            } else if(jsonval.type() == QJsonValue::Object) {
                start += toString(jsonval);
            }
        }

        return start;
    }

    inline static QString toString(const QPixmap pix) {
        QString start = "QPixmap(";
        if(pix.isNull()) {
            start += "null";
        } else {
            start += "has data";
        }

        start += ") ";
        return start;
    }

    template <typename SequentialContainer>
    inline static QString toString(QString which, const SequentialContainer &c)
    {
        QString start = QString("%1(").arg(which);

        typename SequentialContainer::const_iterator it = c.begin(), end = c.end();
        if (it != end) {
            start += *it;
            ++it;
        }

        while (it != end) {
            //debug << ", " << *it;
            start += ", ";
            start += *it;
            ++it;
        }
        //debug << ')';
        start += ')';

        return start;
    }

    template <class T>
    inline static QString toString(const QVector<T> &list)
    {
        return toString( "QVector",list);
    }

    template <typename T, typename Alloc>
    inline static QString toString(const std::vector<T, Alloc> &vec)
    {
        return toString("std::vector", vec);
    }

    template <typename T, typename Alloc>
    inline static QString toString(const std::list<T, Alloc> &vec)
    {
        return toString("std::list", vec);
    }

    template <typename Key, typename T, typename Compare, typename Alloc>
    inline static QString toString(const std::map<Key, T, Compare, Alloc> &map)
    {
        return toString("std::map", map);
    }

    template <typename Key, typename T, typename Compare, typename Alloc>
    inline static QString toString (const std::multimap<Key, T, Compare, Alloc> &map)
    {
        return toString("std::multimap", map);
    }

    template <typename T>
    inline QString toString(QDebug debug, const QSet<T> &set)
    {
        return toString("QSet", set);
    }


    template <class Key, class T>
    inline QString toString(const QMap<Key, T> &map)
    {
        QString start = "QMap(";

        for (typename QMap<Key, T>::const_iterator it = map.constBegin();
             it != map.constEnd(); ++it) {

            start += '(';
            start += it.key();
            start += ", ";
            start += it.value();
            start += ')';
        }

        start +=')';
        return start;
    }


    template <class Key, class T>
    inline static QString toString(const QHash<Key, T> &hash)
    {
        QString start = "QHash(";
        for (typename QHash<Key, T>::const_iterator it = hash.constBegin();
             it != hash.constEnd(); ++it) {
            start += '(';
            start += it.key();
            start += ", ";
            start += it.value();
            start += ')';
        }

        start += ')';

        return start;
    }

    template <class T1, class T2>
    inline static QString toString(const QPair<T1, T2> &pair) {
        QString start = "QPair(";
        start += pair.first;
        start += ',';
        start += pair.second;
        start += ')';
        return start;
    }

    template <class T1, class T2>
    inline static QString toString(const std::pair<T1, T2> &pair)
    {
        QString start = "std::pair(";start += pair.first;
        start += ',';
        start += pair.second;
        start += ')';
        return start;
    }

    template <class T>
    inline static QString toString(const QContiguousCache<T> &cache)
    {
        QString start ="QContiguousCache(";

        for (int i = cache.firstIndex(); i <= cache.lastIndex(); ++i) {
            start += cache[i];
            if (i != cache.lastIndex()) {
                start += ", ";
            }

        }
        start += ")";
        return start;
    }


    static int loglevel;
};

#endif // LOGGER_H
