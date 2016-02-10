#ifndef COPYDIALOG_H
#define COPYDIALOG_H

#include <QDialog>

namespace Ui {
class CopyDialog;
}

class CopyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CopyDialog(QWidget *parent = 0);
    ~CopyDialog();

private:
    Ui::CopyDialog *ui;
};

#endif // COPYDIALOG_H
