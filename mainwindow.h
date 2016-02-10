#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// ====Forward declarations go here====
class AboutDialog;

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

    // Returns true if directory currently being previewed has
    // at least one real item, i.e. directory or file excluding
    // "." or ".." folders. Otherwise, it returns false.
    bool isCurrentDirEmpty();

    // Returns the absolute path of the folder currently shown
    // in files table.
    QString currentFolderAbsolutePath();

    // Returns the absolute path of the currently selected item
    // in files table.
    QString currentItemAbsolutePath();

public slots:
    void onAboutActionTriggered();
    void onCopyActionTriggered();
    void onDeleteActionTriggered();
    void onDetailActionTriggered();
    void onGoUpActionTriggered();
    void onMainExplorerDoubleClicked(const QModelIndex &index);
    void onOpenActionTriggered();
    void onPasteActionTriggered();
    void onSideBarClicked(const QModelIndex &index);

    void popupContextMenu(const QPoint &point);

    void refreshStatusBarCounter();

    // Sets currentIndex of mainExplorer QTableView to the first
    // item inside the folder the view is currently previewing.
    // If that folder is empty, currentIndex is NOT refreshed.
    // If current folder is empty can be checked through
    // isCurrentDirEmpty() public method.
    void setProperMainExplorerCurrentIndex();

    void updateWindowTitle();

signals:
    void mainExplorerRootIndexChanged(const QModelIndex &);

private:
 // Other windows
    AboutDialog *aboutDialog;

 // UI Elements
    Ui::MainWindow *ui;
    QTreeView *sideBar;
    QTableView *mainExplorer;
    QFileSystemModel *sideBarModel;
    QFileSystemModel *mainExplorerModel;

 // Core Elements
    QClipboard *clipboard;

 // Actions
    QAction *exitAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *deleteAction;
    QAction *aboutAction;
    QAction *goUp;
    QAction *openAction;
    QAction *detailAction;

 // Setup Methods
    void setupClipboard();
    void setupMenubarAndToolbar();
    void setupStatusbar();
};

#endif // MAINWINDOW_H
