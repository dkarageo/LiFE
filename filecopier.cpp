#include <QByteArray>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QList>
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
    connect(worker, SIGNAL(error(FileCopier::ErrorType)),
            this, SLOT(emitError(FileCopier::ErrorType)));

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

void FileCopier::emitError(FileCopier::ErrorType err)
{
    emit error(err);
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

void FileCopierWorker::copy(const QString &from, const QString &to)
{
    overallBytesCopied_ = 0;

    bool copied = recursiveCopier(from, to);

    if(copied) emit copyDone();
}

bool FileCopierWorker::recursiveCopier(const QString &from, const QString &to)
{
    QFile fromFile(from, this);
    QFileInfo fromInfo(from);
    bool noErrorOcurred;

    // Handle the case where an invalid source is given.
    if(!fromFile.exists()) {
        emit error(FileCopier::SourceNotExists);
        return false;
    }

    if(fromInfo.isDir()) { // from is a dir, recursively copy its files.
        QDir fromDir(from);
        fromDir.setFilter(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files);

        // If from is a dir path, assuming that to is also a dir path.
        QDir toDir(to);
        // If to dir doesn't exist, create it.
        if(!toDir.exists()) toDir.mkdir(to);

        QDirIterator iter(fromDir);

        while(iter.hasNext()) {
            // For every single entry inside source dir,
            // append its filename to destination dir so
            // the new dest path is created.
            QFileInfo cInfo(iter.next());

            noErrorOcurred = recursiveCopier(
                        cInfo.absoluteFilePath(),
                        to + '/' + cInfo.fileName()
            );

            // If a file fails to copy, don't continue for the rest.
            if(!noErrorOcurred) return noErrorOcurred;
        }
    }
    else { // from is a file, copy it.
        if(!fromFile.open(QFile::ReadOnly)) {
            // Source can't be opened -> General Error
            emit error(FileCopier::OtherError);
            return false;
        }

        QFile toFile(to, this);
        if(toFile.exists()) {
            // Dest file exists so don't overwrite it.
            // Instead give an Exists Error.
            emit error(FileCopier::Exists);

            fromFile.close();
            return false;
        }

        if(!toFile.open(QFile::WriteOnly)) {
            // Dest can't be opened -> General Error
            emit error(FileCopier::OtherError);
            return false;
        }

        if(!toFile.isWritable()) {
            // Dest is not writable -> NotWritable Error
            emit error(FileCopier::NotWritable);

            fromFile.close();
            toFile.close();
            return false;
        }

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
            if(rc != rSize) {
                emit error(FileCopier::OtherError);

                fromFile.close();
                toFile.close();
                delete temp;
                return false;
            }

            rc = toFile.write(temp->data(), rSize);
            if(rc != rSize) {
                emit error(FileCopier::OtherError);

                fromFile.close();
                toFile.close();
                delete temp;
                return false;
            }

            copied += rSize;

            // Adds copy bytes to the overall counter too
            overallBytesCopied_ += rSize;
            emit copyPartlyDone(overallBytesCopied_);
        }

        fromFile.close();
        toFile.close();
        delete temp;
    }

    return true;
}
