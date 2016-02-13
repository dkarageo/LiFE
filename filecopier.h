#ifndef FILECOPIER_H
#define FILECOPIER_H

// ==== Forward declarations go here ====
class CopyDialog;

// ==== Includes go here ====
#include <QObject>
#include <QThread>

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * FileCopier provides an async interface to copy files and folders by
 * optionally previewing the copy status on a gui dialog.
 *
 * Due to its nature, FileCopier should be allocated in the heap
 * and after firing up the copy operation by copy() slot, its status
 * should be retrieved by its signals.
 *
 * sizeCopied(qint64) signal is constantly emitted during copy
 * operation with the bytes copied so far.
 *
 * copied() signal is emitted when the copy operation has finished
 * successfully.
 *
 * On error, error(ErrorType) signal is emitted with the error
 * ocurred.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */
class FileCopier : public QObject
{
    Q_OBJECT
public:
    explicit FileCopier(CopyDialog *dialog = 0, QObject *parent = 0);
    ~FileCopier();

    // Error types that will be emitted by error signal
    typedef enum ErrorType{
        Exists, // Dest file already exists.
        NotWritable, // Dest file is not writable.
        OtherError, // An unexpected error occured.
        SourceNotExists, // Source file doesn't exist.
    } ErrorType;

signals:
    // Emitted only when a file has been successfuly copied.
    void copied();

    // Emitted when an error occured.
    void error(FileCopier::ErrorType);

    // Signal used to fire up worker.
    void operate(const QString &, const QString &);

    // Constantly being emitted while copying, with copied size.
    void sizeCopied(qint64);

public slots:
    // The main slot for starting a copy operation.
    void copy(const QString &from, const QString &to);

private:
    QThread thread_; // Thread where worker should do its job.
    CopyDialog *dialog_; // Window for indicating copy status.

private slots:
    // Slots just used for transfering signals from worker to this.
    void emitCopied();
    void emitError(FileCopier::ErrorType err);
    void emitSizeCopied(qint64 size);

    // Indicator dialog handling slots.
    void openCopyDialog(const QString &from, const QString &to);
    void closeCopyDialog();
};


/*
 * FileCopierWorker used for the solely reason of transfering
 * copy operation to another thread.
 *
 * SHOULD NOT BE USED outside of FileCopier.
 */
class FileCopierWorker : public QObject
{
    Q_OBJECT
public:
    FileCopierWorker(QObject *parent = 0);

signals:
    void copyDone();
    void error(FileCopier::ErrorType);
    void copyPartlyDone(qint64);

public slots:
    void copy(const QString &from, const QString &to);

private:
    bool recursiveCopier(const QString &from, const QString &to);
    qint64 overallBytesCopied_;
};

#endif // FILECOPIER_H
