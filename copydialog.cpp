#include "copydialog.h"
#include "ui_copydialog.h"

CopyDialog::CopyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CopyDialog)
{
    ui->setupUi(this);
}

CopyDialog::~CopyDialog()
{
    delete ui;
}
