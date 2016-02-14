#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

namespace Ui {
class AboutDialog;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * AboutDialog is the main about dialog of LiFE.
 *
 * A simple window that provides info about developer,
 * legal notes etc.
 *
 * Don't forget to keep it up to date!
 *
 * Life is a nightly coding project by dkgr_ser(me)!!!
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */
class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = 0);
    ~AboutDialog();

private:
 // UI properties
    Ui::AboutDialog *ui;

 // Private helper methods
    QString aboutInfo();
};

#endif // ABOUTDIALOG_H
