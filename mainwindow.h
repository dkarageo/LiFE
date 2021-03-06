#ifndef MAINWINDOW_H
#define MAINWINDOW_H

class AboutDialog;

#include <QMainWindow>
#include <QtCore>
#include <QtWidgets>
#include "guifile.h"

namespace Ui {
class MainWindow;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MainWIndow is the base window for LiFE project.
 *
 * It maingly consists of:
 *
 * -mainExplorer(view) & mainExplorerModel(model)
 * -sideBar(view) & mainExplorerModel(model)
 *  These two should be kept synchronized, and while one is updated
 *  the other should be updated too.
 *
 * -Actions and their handlers:
 *  Actions created in the code, should be referenced under
 *  private members Actions section for later usage.
 *
 *  Adding an action means:
 *      -Create a reference under private members.
 *      -Creating action with its icon.
 *      -Adding that action to menubar, toolbar, contextMenu.
 *
 * -Other UI Elements:
 *  Every UI Element that may be later called again, should have
 *  a reference under private mebers UI Elements section. *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */
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
    void onNewFolderAction();
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
    QAction *newFolderAction;
    QAction *detailAction;

 // Setup Methods
    void setupClipboard();
    void setupMenubarAndToolbar();
    void setupStatusbar();

 // Private members used for paste operation.
    typedef struct {
        FileCopier::ErrorType err;
        GuiFile *file;
        QString to;
    } UnhandledCopyError;

    QLinkedList<UnhandledCopyError> *errors_;

    bool yestoall_;
    bool notoall_;
    bool existsDialogOpen_;
    bool errorsAlreadyHandled_;

private slots:
    // Used to handle errors occured during file pasting.
    void handlePasteError(FileCopier::ErrorType, GuiFile *, const QString &to);
};

#endif // MAINWINDOW_H
