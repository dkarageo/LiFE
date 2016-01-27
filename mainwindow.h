#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QtWidgets>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void onAboutActionTriggered();
    void onCopyActionTriggered();
    void onDeleteActionTriggered();
    void onGoUpActionTriggered();
    void onMainExplorerDoubleClicked(const QModelIndex &index);
    void onPasteActionTriggered();
    void onSideBarClicked(const QModelIndex &index);
    void refreshStatusBarCounter();

signals:
    void mainExplorerRootIndexChanged(const QModelIndex &);

private:
 // UI Elements
    Ui::MainWindow *ui;
    QTreeView *sideBar;
    QTableView *mainExplorer;
    QFileSystemModel *sideBarModel;
    QFileSystemModel *mainExplorerModel;

 // Core Elements
    QClipboard *clipboard;

 // Setup Methods
    void setupClipboard();
    void setupMenubarAndToolbar();
    void setupStatusbar();
};

#endif // MAINWINDOW_H
