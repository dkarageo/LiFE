#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
#include "enhancedqt.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

// Creating and configuring essential UI Widgets.
    ui->centralWidget->setWindowTitle("LiFE");

    // Creating the main layout of the centralWidget.
    QHBoxLayout *layout = new QHBoxLayout(this);
    ui->centralWidget->setLayout(layout);

    // Creating the two main views of the UI
    sideBar = new QTreeView(this);
    sideBar->header()->hide();

    mainExplorer = new QTableView(this);
    mainExplorer->verticalHeader()->hide();

    // Adding the two main views inside a splitter
    QSplitter *splitter = new QSplitter(this);
    splitter->addWidget(sideBar);
    splitter->addWidget(mainExplorer);

    layout->addWidget(splitter); // and add splitter to layout

    // Setting initial values for the initial splitting
    QList<int> splittedSizes;
    splittedSizes << 300 << 700;
    splitter->setSizes(splittedSizes);

// Creating and configuring essential models.
    sideBarModel = new QFileSystemModel(this);
    mainExplorerModel = new QFileSystemModel(this);

    // Making models to allow filesystem writing.
    sideBarModel->setReadOnly(false);
    mainExplorerModel->setReadOnly(false);

    QString initialRootPath = "/";

    // Creating data models for sideBar and mainExplorer
    sideBarModel->setRootPath(initialRootPath);
    sideBarModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);

    mainExplorerModel->setRootPath(initialRootPath);
    mainExplorerModel->setFilter(QDir::NoDotAndDotDot | QDir::AllEntries);

    // Adding models to views.
    sideBar->setModel(sideBarModel);
    mainExplorer->setModel(mainExplorerModel);

// Reconfiguring widgets after attaching models to them.
    sideBar->hideColumn(1);
    sideBar->hideColumn(2);
    sideBar->hideColumn(3);

// Setting up additional elements
    setupClipboard();
    setupMenubarAndToolbar();
    setupStatusbar();

// SIGNAL-SLOT connections go here
    connect(sideBar, SIGNAL(clicked(QModelIndex)),
            this, SLOT(onSideBarClicked(QModelIndex)));
    connect(mainExplorer, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(onMainExplorerDoubleClicked(QModelIndex)));
}

MainWindow::~MainWindow()
{
    delete ui;
}


// ==== Implementations of private setup methods go here ====

void MainWindow::setupClipboard()
{
    clipboard = QApplication::clipboard();
}

void MainWindow::setupMenubarAndToolbar()
{
    QMenu *fileMenu = new QMenu("File", this);
    QMenu *editMenu = new QMenu("Edit", this);
    QMenu *helpMenu = new QMenu("Help", this);

    ui->menuBar->addMenu(fileMenu);
    ui->menuBar->addMenu(editMenu);
    ui->menuBar->addMenu(helpMenu);

// File Menu Actions
    QAction *exitAction = new QAction(
            QIcon(":/_Images/Icons/Menu/exit.png"), "Exit", this);

    fileMenu->addAction(exitAction);

    connect(exitAction, SIGNAL(triggered(bool)), this, SLOT(close()));

// Edit Menu Actions
    QAction *copyAction = new QAction(
            QIcon(":/_Images/Icons/Menu/copy.png"), "Copy", this);
    QAction *pasteAction = new QAction(
            QIcon(":/_Images/Icons/Menu/paste.png"), "Paste", this);
    QAction *deleteAction = new QAction(
            QIcon(":/_Images/Icons/Menu/delete.png"), "Delete", this);

    editMenu->addAction(copyAction);
    editMenu->addAction(pasteAction);
    editMenu->addAction(deleteAction);

    connect(copyAction, SIGNAL(triggered(bool)),
            this, SLOT(onCopyActionTriggered()));
    connect(pasteAction, SIGNAL(triggered(bool)),
            this, SLOT(onPasteActionTriggered()));
    connect(deleteAction, SIGNAL(triggered(bool)),
            this, SLOT(onDeleteActionTriggered()));

// Help Menu Actions
    QAction *aboutAction = new QAction(
            QIcon(":/_Images/Icons/Menu/about.png"), "About", this);

    helpMenu->addAction(aboutAction);

    connect(aboutAction, SIGNAL(triggered(bool)),
            this, SLOT(onAboutActionTriggered()));

// Adding Useful Actions to mainToolbar  
    ui->mainToolBar->addAction(copyAction);
    ui->mainToolBar->addAction(pasteAction);
    ui->mainToolBar->addAction(deleteAction);

    QAction *goUp = new QAction(
            QIcon(":/_Images/Icons/Menu/upper_directory.png"), "Up", this);
    ui->mainToolBar->insertAction(copyAction, goUp);

    connect(goUp, SIGNAL(triggered(bool)),
            this, SLOT(onGoUpActionTriggered()));
}

void MainWindow::setupStatusbar()
{
    refreshStatusBarCounter();

    // Connecting signals that mean permStatusBarCounter should be refreshed.
    connect(this, SIGNAL(mainExplorerRootIndexChanged(QModelIndex)),
            this, SLOT(refreshStatusBarCounter()));
}


// ==== Implementations of slots go here ====

void MainWindow::onAboutActionTriggered()
{
    aboutDialog = new AboutDialog;
    aboutDialog->setModal(true);
    aboutDialog->exec();
}

void MainWindow::onCopyActionTriggered()
// Well, Qt IS NOT cross-platform here. GNOME got non-standard mime type
// for different files, the same for other windows manager, the same on OSX.
// Current QMimeDatabase is shitty... and compatible with nothing...
{
    // Get current index.
    QModelIndex cIndex = mainExplorer->currentIndex();
    if(!cIndex.isValid()) return;

    // Get path of file to be copied.
    QString cPath = mainExplorerModel->fileInfo(cIndex).absoluteFilePath();

    // Create QMimeData object, that works on Windows and wherever lucky!
    QMimeData *data = new QMimeData();

    QList<QUrl> urlsList;
    urlsList.append(QUrl::fromLocalFile(cPath));

    data->setUrls(urlsList);

    clipboard->setMimeData(data);
}

void MainWindow::onDeleteActionTriggered()
{
    QModelIndex cIndex = mainExplorer->currentIndex();
    if(!cIndex.isValid()) return;

    QFileInfo cFile = mainExplorerModel->fileInfo(cIndex);

    QMessageBox::StandardButton choice;

    // Display question dialog only if there is something to remove
    if((cFile.isDir() | cFile.isFile()) && cFile.isWritable()) {
        choice = QMessageBox::question(
                    this, "Confirm Deletion",
                    "Do you want to permanently remove " +
                    cFile.completeBaseName() + "." +
                    cFile.completeSuffix() + "?",
                    QMessageBox::No | QMessageBox::Yes
        );
    }
    else return;

    // Simply return if user selects No.
    if(choice == QMessageBox::No) return;

    if(cFile.isDir()) {
        mainExplorerModel->rmdir(cIndex);
    }
    else if(cFile.isFile()){
        mainExplorerModel->remove(cIndex);
    }
}

void MainWindow::onGoUpActionTriggered()
{
    QModelIndex cIndex = mainExplorer->currentIndex();
    if(!cIndex.isValid()) return;

    // Using QDir to go to above dir, instead of unsafely using
    // QModelIndexes.
    QDir cDir = mainExplorerModel->fileInfo(cIndex).dir();
    cDir.cdUp();
    QString upperPath = cDir.absolutePath();

    QModelIndex sideBarIndex = sideBarModel->index(upperPath);
    sideBar->setCurrentIndex(sideBarIndex);
    sideBar->scrollTo(sideBarIndex);

    QModelIndex mainExplorerIndex = mainExplorerModel->setRootPath(upperPath);
    mainExplorer->setRootIndex(mainExplorerIndex);

    emit mainExplorerRootIndexChanged(mainExplorerIndex);
}

void MainWindow::onMainExplorerDoubleClicked(const QModelIndex &index)
{
    // Getting path of folder whose contents mainExplorer should show.
    QString path = mainExplorerModel->fileInfo(index).absoluteFilePath();

    mainExplorer->setRootIndex(mainExplorerModel->setRootPath(path));
    emit mainExplorerRootIndexChanged(mainExplorer->rootIndex());

    mainExplorer->setCurrentIndex(mainExplorerModel->index(path).child(1,1));

    // Keep sideBar synchronized too.
    QModelIndex sideBarIndex = sideBarModel->index(path);
    sideBar->expand(sideBarIndex);
    sideBar->scrollTo(sideBarIndex, QAbstractItemView::PositionAtTop);
}

void MainWindow::onPasteActionTriggered()
{
    // Get the urls list from clipboard, if any.
    if(!clipboard->mimeData()->hasUrls()) return;

    QList<QUrl> urls = clipboard->mimeData()->urls();

    // Get current folder's path, which is the parent folder of
    // currently selected index in mainExplorer view.
    QString destPath = mainExplorerModel->fileInfo(
                mainExplorer->currentIndex()).canonicalPath() + "/";

    bool yestoall = false;
    bool notoall = false;

    foreach(QUrl url, urls) {
        QString fPath = url.toLocalFile(); // Translate url to absolute path.
        QFileInfo fileInfo(url.toLocalFile());

        // Create the final absolute path, where fPath file will be copied.
        QString cFinalPath = destPath + fileInfo.completeBaseName() +
                             "." + fileInfo.completeSuffix();

        // For each file attempt to copy it
        if(!QFile::copy(fPath, cFinalPath)) {
            // copy returned false - handle same filename

            if(yestoall) {
                // "YesToAll" selected, so don't notify user anymore.
                if(threadsafeFileRemove(cFinalPath)) {
                    QFile::copy(fPath, cFinalPath);
                }
            }
            else if(notoall) {
                // "NoToAll" selected, so just do nothing for this file.
            }
            else {
                // No "YesToAll" selected, so ask user what to do.
                QMessageBox::StandardButton choice = QMessageBox::question(
                        this, "How to copy file?",
                        "File \"" + fileInfo.completeBaseName() +
                        "." + fileInfo.completeSuffix() +
                        "\" already exists. Do you want to overwrite it?",
                        QMessageBox::No | QMessageBox::NoToAll | QMessageBox::Yes | QMessageBox::YesToAll
                );

                if(choice == QMessageBox::Yes) {
                    if(threadsafeFileRemove(cFinalPath)) {
                        QFile::copy(fPath, cFinalPath);
                    }
                }
                else if(choice == QMessageBox::YesToAll) {
                    yestoall = true;

                    if(threadsafeFileRemove(cFinalPath)) {
                        QFile::copy(fPath, cFinalPath);
                    }
                }
                else if(choice == QMessageBox::NoToAll) {
                    notoall = true;
                }
                // "No" or anything else - nothing to do, so no else
            }
        }
    }
}

void MainWindow::onSideBarClicked(const QModelIndex &index)
{
    // Getting path of folder whose contents mainExplorer should show.
    QString path = sideBarModel->fileInfo(index).absoluteFilePath();

    mainExplorer->setRootIndex(mainExplorerModel->setRootPath(path));
    emit mainExplorerRootIndexChanged(mainExplorer->rootIndex());
}

void MainWindow::refreshStatusBarCounter()
{
    // Getting directory set as root directory in mainExplorerModel.
    QDir cDir = mainExplorerModel->rootDirectory();
    cDir.setFilter(QDir::NoDotAndDotDot | QDir::AllEntries);

    ui->statusBar->showMessage(QString::number(cDir.count()) + " objects");
}
