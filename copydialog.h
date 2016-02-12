#ifndef COPYDIALOG_H
#define COPYDIALOG_H

// ====Forward declarations go here====
class QProgressBar;
class QLabel;

// ====Includes go here====
#include <QDialog>
#include <QFile>

namespace Ui {
class CopyDialog;
}


class CopyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CopyDialog(const QFile &from, const QFile &to, QWidget *parent = 0);
    ~CopyDialog();

public slots:
    void setSizeCopied(qint64 size);

private:
 // UI Elements
    Ui::CopyDialog *ui;
    QProgressBar *prog_;
    QLabel *status_;

    qint64 size_;
};

#endif // COPYDIALOG_H
