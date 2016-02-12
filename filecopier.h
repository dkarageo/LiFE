#ifndef FILECOPIER_H
#define FILECOPIER_H

class CopyDialog;

#include <QObject>
#include <QThread>

class FileCopier : public QObject
{
    Q_OBJECT

public:
    explicit FileCopier(CopyDialog *dialog = 0, QObject *parent = 0);
    ~FileCopier();

signals:
    void copied();
    void operate(const QString &, const QString &);
    void sizeCopied(qint64);

public slots:
    void copy(const QString &from, const QString &to);
    void openCopyDialog(const QString &from, const QString &to);
    void closeCopyDialog();

private:
    QThread thread_;
    CopyDialog *dialog_;

private slots:
    void emitCopied();
    void emitSizeCopied(qint64 size);
};


class FileCopierWorker : public QObject
{
    Q_OBJECT

public:
    FileCopierWorker(QObject *parent = 0);

signals:
    void copyDone();
    void copyPartlyDone(qint64);

public slots:
    int copy(const QString &from, const QString &to);
};

#endif // FILECOPIER_H
