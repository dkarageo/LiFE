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
    void onSideBarClicked(const QModelIndex &index);
    void onMainExplorerDoubleClicked(const QModelIndex &index);
    void onMainExplorerRowCountChanged(int rowCount);

private:
    Ui::MainWindow *ui;
    QTreeView *sideBar;
    QTableView *mainExplorer;
    QFileSystemModel *sideBarModel;
    QFileSystemModel *mainExplorerModel;
    void setupMenubar();
    void setupToolbar();
    void setupStatusbar();
};

#endif // MAINWINDOW_H
