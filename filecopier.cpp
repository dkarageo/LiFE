#include <QFile>
#include <QByteArray>
#include "copydialog.h"
#include "filecopier.h"
#include <QDebug>

FileCopier::FileCopier(CopyDialog *dialog, QObject *parent) :
    QObject(parent)
{
    dialog_ = dialog;

    FileCopierWorker *worker = new FileCopierWorker;
    worker->moveToThread(&thread_);

    // Start-Stop signals
    connect(&thread_, &QThread::finished,
            worker, &QObject::deleteLater);
    connect(this, SIGNAL(operate(QString, QString)),
            worker, SLOT(copy(QString, QString)));

    // Copy indicator signals
    connect(worker, SIGNAL(copyDone()),
            this, SLOT(emitCopied()));
    connect(worker, SIGNAL(copyPartlyDone(qint64)),
            this, SLOT(emitSizeCopied(qint64)));

    // Gui signals
    connect(this, SIGNAL(operate(QString,QString)),
            this, SLOT(openCopyDialog(QString,QString)));
    connect(worker, SIGNAL(copyDone()),
            this, SLOT(closeCopyDialog()));

    thread_.start();
}

FileCopier::~FileCopier()
{
    thread_.quit();
    thread_.wait();
}

void FileCopier::copy(const QString &from, const QString &to)
{
    emit operate(from, to);
}

void FileCopier::openCopyDialog(const QString &from, const QString &to)
{
    // If no copy dialog given, use the default one.
    if(!dialog_) {
        dialog_ = new CopyDialog(QFile(from), QFile(to));
    }

    connect(this, SIGNAL(sizeCopied(qint64)),
            dialog_, SLOT(setSizeCopied(qint64)));

    dialog_->show();
}

void FileCopier::closeCopyDialog()
{
    dialog_->close();
    delete dialog_;
}

void FileCopier::emitCopied()
{
    emit copied();
}

void FileCopier::emitSizeCopied(qint64 size)
{
    emit sizeCopied(size);
}


FileCopierWorker::FileCopierWorker(QObject *parent):
    QObject(parent)
{
    // Nothing here
}

int FileCopierWorker::copy(const QString &from, const QString &to)
{
    QFile fromFile(from, this);
    if(!fromFile.exists()) return -1;
    if(!fromFile.open(QFile::ReadOnly)) return -1;

    QFile toFile(to, this);
    if(!toFile.open(QFile::WriteOnly)) return -1;

    qint64 size = fromFile.size();
    qint64 copied = 0;

    // Copy in chuncks of 0.5 MB
    qint64 chunkSize = 1024 * 512;

    QByteArray *temp = new QByteArray;
    temp->resize(1024 * 512);

    while(copied < size) {
        // Reduce the chunk to be copied, when less than chunkSize bytes left.
        qint64 rSize = size - copied > chunkSize ? chunkSize : size - copied;

        qint64 rc = fromFile.read(temp->data(), rSize);
        if(rc != rSize) return -1;

        rc = toFile.write(temp->data(), rSize);
        if(rc != rSize) return -1;

        copied += rSize;

        emit copyPartlyDone(copied);
    }

    emit copyDone();

    fromFile.close();
    toFile.close();
    delete temp;

    return 0;
}
