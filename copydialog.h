#ifndef COPYDIALOG_H
#define COPYDIALOG_H

class QProgressBar;
class QLabel;

#include <QDialog>
#include <QFile>

namespace Ui {
class CopyDialog;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * CopyDialog provides a simple dialog that displays progress
 * of a file being copied.
 *
 * It is intended to be used by used by any async copy operation,
 * with the only requirment to connect a signal that provides
 * the copied size at regular intervals to setSize(qint64) slot.
 *
 * That interval should be at a rate that a QWidget is able to handle.
 *
 * Filenames, sizes etc. will be set according to QFiles give to the
 * constructor.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */
class CopyDialog : public QDialog
{
    Q_OBJECT

public:
    // Create the CopyDialog by giving a QFile that represents source file,
    // and a QFile that represent destination file.
    explicit CopyDialog(const QFile &from, const QFile &to, QWidget *parent = 0);
    ~CopyDialog();

public slots:
    // Updates status with the given size. It should be the bytes copied so far.
    void setSizeCopied(qint64 size);

private:
 // UI Elements
    Ui::CopyDialog *ui;
    QProgressBar *prog_;
    QLabel *status_;

 // Helpers
    qint64 size_;

private slots:
    void setSize(qint64 size);
};

#endif // COPYDIALOG_H
