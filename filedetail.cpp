#include <QtCore>
#include <QBoxLayout>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QLabel>
#include <QLineEdit>
#include <QSpacerItem>
#include "dirsizecounter.h"
#include "filedetail.h"
#include "ui_filedetail.h"


FileDetail::FileDetail(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileDetail)
{
    ui->setupUi(this);

    setupLayout();
    doConnections();
}

FileDetail::FileDetail(QFile *file, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileDetail)
{
    ui->setupUi(this);

    setupLayout();
    doConnections();

    setFile(file);
}

FileDetail::~FileDetail()
{
    delete ui;
}

QFile *FileDetail::file() const
{
    return file_;
}


// ====Private setup methods go here====

void FileDetail::doConnections()
{
    connect(ui->buttonBox, SIGNAL(accepted()),
            this, SLOT(renameFile()));
}

void FileDetail::setupLayout()
{
    // Configuring dialog
    this->setFixedSize(400, 450);

    // Setting a vertical layout for the whole window
    QVBoxLayout *dialogLayout = new QVBoxLayout(this);
    this->setLayout(dialogLayout);

    // Setting up file icon and name.
    icon_ = new QLabel(this);

    QLabel *fileNameLabel = new QLabel("Name: ", this);
    fileName_ = new QLineEdit(this);
    QHBoxLayout *iconAndName = new QHBoxLayout(this);

    iconAndName->addWidget(icon_);
    iconAndName->addWidget(fileNameLabel);
    iconAndName->addWidget(fileName_);

    dialogLayout->addLayout(iconAndName);

    // Setting up fileType
    QLabel *fileTypeLabel = new QLabel("File Type: ", this);
    fileType_ = new QLabel(this);

    QHBoxLayout *fileTypeLayout = new QHBoxLayout(this);

    fileTypeLayout->addWidget(fileTypeLabel);
    fileTypeLayout->addWidget(fileType_);

    dialogLayout->addLayout(fileTypeLayout);

    // Setting up path
    QLabel *pathLabel = new QLabel("Path: ", this);
    path_ = new QLabel(this);

    QHBoxLayout *pathLayout = new QHBoxLayout(this);

    pathLayout->addWidget(pathLabel);
    pathLayout->addWidget(path_);

    dialogLayout->addLayout(pathLayout);

    // Setting up size
    QLabel *sizeLabel = new QLabel("Size: ", this);
    size_ = new QLabel(this);

    QHBoxLayout *sizeLayout = new QHBoxLayout(this);

    sizeLayout->addWidget(sizeLabel);
    sizeLayout->addWidget(size_);

    dialogLayout->addLayout(sizeLayout);

    // Setting up dateCreated
    QLabel *dateCreatedLabel = new QLabel("Date Created: ", this);
    dateCreated_ = new QLabel(this);

    QHBoxLayout *dateCreatedLayout = new QHBoxLayout(this);

    dateCreatedLayout->addWidget(dateCreatedLabel);
    dateCreatedLayout->addWidget(dateCreated_);

    dialogLayout->addLayout(dateCreatedLayout);

    // Setting up dateModified
    QLabel *dateModifiedLabel = new QLabel("Date Modified: ", this);
    dateModified_ = new QLabel(this);

    QHBoxLayout *dateModifiedLayout = new QHBoxLayout(this);

    dateModifiedLayout->addWidget(dateModifiedLabel);
    dateModifiedLayout->addWidget(dateModified_);

    dialogLayout->addLayout(dateModifiedLayout);

    // Add a spacer before buttons
    QSpacerItem *spacer = new QSpacerItem(1, 170, QSizePolicy::Fixed, QSizePolicy::Fixed);
    dialogLayout->addSpacerItem(spacer);

    // And finally add auto generated buttons
    dialogLayout->addWidget(ui->buttonBox);
}


// ====Slots go here====

void FileDetail::renameFile()
{
    if((fileName_->text() != file_->fileName()) && file_->isWritable()) {
        file_->rename(fileName_->text());
    }
}

void FileDetail::setFile(QFile *file)
{
    file_ = file;
    QFileInfo fileInfo(*file);

    QFileIconProvider ip;
    QIcon cIcon = ip.icon(fileInfo);
    setIcon(cIcon);

    setFileName(fileInfo.fileName());

    if(fileInfo.isDir()) {
        setFileType("Files folder");
    }
    else {
        setFileType(fileInfo.completeSuffix());
    }

    setPath(fileInfo.absoluteFilePath());

    if(fileInfo.isDir()) {
        // FileDetail dialog doesn't stay open forever
        // so memory leak of this in case dialog updated with a new file
        // should not be a problem. Memory gonna be released when closed.
        DirSizeCounter *counter = new DirSizeCounter(this);

        connect(counter, SIGNAL(sizePartlyCounted(qint64)),
                this, SLOT(setSize(qint64)));
        connect(counter, SIGNAL(sizeCounted(qint64)),
                this, SLOT(setSize(qint64)));

        counter->countSize(fileInfo.filePath());
    }
    else {
        setSize(fileInfo.size());
    }

    setDateCreated(fileInfo.created());

    setDateModified(fileInfo.lastModified());
}

void FileDetail::setIcon(const QIcon &icon)
{
    icon_->setPixmap(icon.pixmap(20, 20));
    this->setWindowIcon(icon);
}

void FileDetail::setFileName(const QString &name)
{
    fileName_->setText(name);
    this->setWindowTitle("Properties: " + name);
}

void FileDetail::setFileType(const QString &type)
{
    fileType_->setText(type);
}

void FileDetail::setPath(const QString &path)
{
    path_->setText(path);
}

void FileDetail::setSize(qint64 size)
{
    double value;
    QString unit;

    // Special treat for 0 bytes files - show less text.
    if(size == 0) {
        size_->setText("0 Bytes");
        return;
    }

    if((size / 1024) < 1024) {
        value = (double)size / 1024;
        unit = "KB";
    }
    else if((size / (1024 * 1024)) < 1024) {
        value = (double)size / (1024 * 1024);
        unit = "MB";
    }
    else if((size / (1024 * 1024 * 1024)) < 1024 ) {
        value = (double)size / (1024 * 1024 * 1024);
        unit = "GB";
    }
    else {
        value = (double)size / ((qint64)1024 * 1024 * 1024 * 1024);
        unit = "TB";
    }

    size_->setText(QString::number(value, 'f', 2) + unit +
                   " (" +  QString::number(size) + " Bytes)");
}

void FileDetail::setDateCreated(const QDateTime &created)
{
    dateCreated_->setText(created.toString());
}

void FileDetail::setDateModified(const QDateTime &modified)
{
    dateModified_->setText(modified.toString());
}
