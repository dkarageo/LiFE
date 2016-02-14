#ifndef DIRSIZECOUNTER_H
#define DIRSIZECOUNTER_H

class QTimer;

#include <QObject>
#include <QThread>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * DirSizeCounter provides a non-blocking way for counting
 * the size of a directory with complex subdirectory scheme
 * with many subfolders and files.
 *
 * Result can be counted by using countSize(const QString &path).
 *
 * Every time a subfolder is counted sizePartlyCounted signal is
 * emitted.
 *
 * When result has been completely counted sizeCounted signal
 * is emitted.
 *
 * Due to its async nature it should be allocated in heap.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */
class DirSizeCounter : public QObject
{
    Q_OBJECT

public:
    explicit DirSizeCounter(QObject *parent = 0);
    ~DirSizeCounter();

signals:
    // Signal emitted by count size and connected to worker's thread.
    void operate(const QString &);

    // Signal emitted when the size has been fully counted.
    void sizeCounted(qint64);

    // Signal emitted everytime a subdir's size has been counted,
    // until size is completely counted.
    void sizePartlyCounted(qint64);

public slots:
    // The main function that fires up size counting for the
    // given path, that should be a dir's path.
    void countSize(const QString &path);

private:
    QThread thread_;

private slots:
    // Slots used for passing signals from worker to this.
    void emitSizeCounted(qint64);
    void emitSizePartlyCounted(qint64);
};


/*
 * DirSizeCounterWorker should not be used outside
 * DirSizeCounter. Its only job is to be moved to another
 * thread for async work.
 */
class DirSizeCounterWorker : public QObject
{
    Q_OBJECT

public:
    explicit DirSizeCounterWorker(QObject *parent = 0);

signals:
    void resultReady(qint64);
    void resultPartlyReady(qint64);

public slots:
    qint64 countSize(const QString &path);

private:
    qint64 size_;

    qint64 recursiveCounter(const QString &path);
};

#endif // DIRSIZECOUNTER_H
