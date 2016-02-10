#ifndef DIRSIZECOUNTER_H
#define DIRSIZECOUNTER_H

class QTimer;

#include <QObject>
#include <QThread>

/*
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
 */
class DirSizeCounter : public QObject
{
    Q_OBJECT

public:
    explicit DirSizeCounter(QObject *parent = 0);
    ~DirSizeCounter();

signals:
    void operate(const QString &);
    void sizeCounted(qint64);
    void sizePartlyCounted(qint64);

public slots:
    void countSize(const QString &path);

    void emitSizeCounted(qint64);
    void emitSizePartlyCounted(qint64);

private:
    QThread thread_;
};


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
