#include <QBoxLayout>
#include <QFileInfo>
#include <QLabel>
#include <QProgressBar>
#include "copydialog.h"
#include "ui_copydialog.h"

CopyDialog::CopyDialog(const QFile &from, const QFile &to, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CopyDialog)
{
    ui->setupUi(this);

    QFileInfo fromInfo(from);
    QFileInfo toInfo(to);

    // Keep file's size for later usage.
    size_ = fromInfo.size();

// Setting window properties
    // Creating a vertical layout for the whole window.
    QVBoxLayout *windowLayout = new QVBoxLayout(this);
    this->setLayout(windowLayout);

    // Indicate in title the file being copied.
    this->setWindowTitle("Copying: " + from.fileName());

    // Setting window icon to the program's one
    this->setWindowIcon(QIcon(":/_Images/Icons/Logo/logo.png"));

// Setting window elements
    prog_ = new QProgressBar(this);
    status_ = new QLabel;

    prog_->setMaximum(100);
    prog_->setValue(0);

    status_->setText("Copied: 0 out of " + QString::number(fromInfo.size()) + " bytes.");

    windowLayout->addWidget(prog_);
    windowLayout->addWidget(status_);

    // Creating two labels that gives more info about paths
    QLabel *fromLabel = new QLabel(this);
    QLabel *toLabel = new QLabel(this);

    fromLabel->setText("From: " + fromInfo.completeBaseName() + '.' + fromInfo.completeSuffix());
    toLabel->setText("To: " + toInfo.completeBaseName() + '.' + toInfo.completeSuffix());

    windowLayout->addWidget(fromLabel);
    windowLayout->addWidget(toLabel);
}

CopyDialog::~CopyDialog()
{
    delete ui;
}

void CopyDialog::setSizeCopied(qint64 size)
{
    prog_->setValue((size * 100) / size_);

    status_->setText("Copied:" + QString::number(size) +
                     " out of " + QString::number(size_) +
                     " bytes.");
}
