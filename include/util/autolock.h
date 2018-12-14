#ifndef AUTOLOCK_H
#define AUTOLOCK_H

#include <QMutex>

class AutoLock {

public:
    inline AutoLock(QMutex& mutex) : mLock(&mutex)  { mLock->lock(); }
    inline AutoLock(QMutex* mutex) : mLock(mutex) { mLock->lock(); }
    inline ~AutoLock() { mLock->unlock(); }

private:
    QMutex *mLock;
};

#endif // AUTOLOCK_H
