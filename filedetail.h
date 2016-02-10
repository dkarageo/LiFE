#ifndef FILEDETAIL_H
#define FILEDETAIL_H

// ====Forward declarations go here====
class QFile;
class QLabel;
class QLineEdit;
class DirSizeCounter;

// ====Includes go here====
#include <QDialog>


namespace Ui {
class FileDetail;
}


class FileDetail : public QDialog
{
    Q_OBJECT

public:
    explicit FileDetail(QWidget *parent = 0);
    explicit FileDetail(QFile *file, QWidget *parent = 0);
    ~FileDetail();

    // Returns a pointer to file currently associated with FileDetail object.
    QFile *file() const;

public slots:
    // If user changed name inside filename field of the dialog,
    // rename the file accordingly.
    void renameFile();

    // Associates a file object with FileDetail.
    void setFile(QFile *file);

 // Slots for setting private members
    void setIcon(const QIcon &icon);
    void setFileName(const QString &name);
    void setFileType(const QString &type);
    void setPath(const QString &path);
    void setSize(qint64 size);
    void setDateCreated(const QDateTime &created);
    void setDateModified(const QDateTime &modified);

private:
 // UI properties
    Ui::FileDetail *ui;
    QLabel *icon_;
    QLineEdit *fileName_;
    QLabel *fileType_;
    QLabel *path_;
    QLabel *size_;
    QLabel *dateCreated_;
    QLabel *dateModified_;

 // Core elements
    QFile *file_;
    QThread *thread_;

 // Private setup methods
    void doConnections();
    void setupLayout();
};


#endif // FILEDETAIL_H
