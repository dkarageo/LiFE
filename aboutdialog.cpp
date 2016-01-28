#include <QLabel>
#include <QString>
#include <QBoxLayout>
#include <QPushButton>
#include <QPixmap>
#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

// Configuring dialog's main attributes.
    this->setFixedSize(440, 230);
    this->setWindowTitle("About");

    // Loading application's logo, and setting window icon.
    QPixmap *image = new QPixmap(":/_Images/Icons/Logo/logo.png");
    this->setWindowIcon(QIcon(*image));

    // Creating a vertical layout for the entire dialog.
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    this->setLayout(mainLayout);

// Creating contents of the About dialog.
    // Creating two horizontal columns, one for logo, one for info.
    QHBoxLayout *contentLayout = new QHBoxLayout(this);

    QLabel *info = new QLabel(this);
    QLabel *logo = new QLabel(this);

    contentLayout->addWidget(logo);
    contentLayout->addWidget(info);

    info->setText(aboutInfo());

    logo->setPixmap(image->scaledToWidth(180)); // im

    mainLayout->addLayout(contentLayout);

    // Creating the OK buton.
    QPushButton *button = new QPushButton("Ok", this);
    button->setMaximumWidth(60);

    mainLayout->addWidget(button, 0, Qt::AlignCenter);

// Connecting SIGNALS-SLOTS.
    connect(button, SIGNAL(clicked(bool)),
            this, SLOT(close()));
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

QString AboutDialog::aboutInfo()
{
    QString infoText;

    infoText.append("Created by <b>Dimitrios Karageorgiou</b>. <br />");
    infoText.append("Email: <span color=blue>dkgr_ser@outlook.com</span> <br />");
    infoText.append("<br />");
    infoText.append("As an educational project during the course: <br />");
    infoText.append("\"<i>Programatic Techniques</i>\" <br />");
    infoText.append("<br />");
    infoText.append("Public repo: \"<a href=\"https://bitbucket.org/dkgr_ser/life\">https://bitbucket.org/dkgr_ser/life</a>\"<br />");
    infoText.append("<br />");
    infoText.append("Released under GPLv3.");

    return infoText;
}
