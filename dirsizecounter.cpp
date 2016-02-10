#include <QDir>
#include <QDirIterator>
#include <QThread>
#include <QTimer>
#include "dirsizecounter.h"
#include <QDebug>

DirSizeCounter::DirSizeCounter(QObject *parent) :
    QObject(parent)
{
    DirSizeCounterWorker *worker = new DirSizeCounterWorker;
    worker->moveToThread(&thread_);

    connect(&thread_, &QThread::finished,
            worker, &QObject::deleteLater);
    connect(this, SIGNAL(operate(const QString &)),
            worker, SLOT(countSize(const QString &)));
    connect(worker, SIGNAL(resultPartlyReady(qint64)),
            this, SLOT(emitSizePartlyCounted(qint64)));
    connect(worker, SIGNAL(resultReady(qint64)),
            this, SLOT(emitSizeCounted(qint64)));

    thread_.start();
}

DirSizeCounter::~DirSizeCounter()
{
    thread_.quit();
    thread_.wait();
}

void DirSizeCounter::countSize(const QString &path)
{
    emit operate(path);
}

void DirSizeCounter::emitSizePartlyCounted(qint64 res)
{
    emit sizePartlyCounted(res);
}

void DirSizeCounter::emitSizeCounted(qint64 res)
{
    emit sizeCounted(res);
}


DirSizeCounterWorker::DirSizeCounterWorker(QObject *parent) :
    QObject(parent)
{
    // Nothing here
}

qint64 DirSizeCounterWorker::countSize(const QString &path)
{
    size_ = 0;

    size_ = recursiveCounter(path);

    emit resultReady(size_);

    return size_;
}

qint64 DirSizeCounterWorker::recursiveCounter(const QString &path)
{
    QDir dir(path);
    dir.setFilter(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs);

    QDirIterator iter(dir);

    qint64 size = 0;

    while(iter.hasNext()) {
        QString nPath = iter.next();

        QFileInfo nInfo(nPath);

        if(nInfo.isDir()) {
            size += recursiveCounter(nPath);
        }
        else {
            qint64 tSize = nInfo.size();
            size += tSize;

            // Don't wanna think the non recursive equivalent, so size_
            // is just a duplicate size tracker outside recursion.
            size_ += tSize;
        }
    }

    emit resultPartlyReady(size_);

    return size;
}
