#include "mainwindow.h"
#include "ui_mainwindow.h"


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

// Setting up additional UI elements
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

    connect(deleteAction, SIGNAL(triggered(bool)),
            this, SLOT(onDeleteActionTriggered()));

// Help Menu Actions
    QAction *aboutAction = new QAction(
            QIcon(":/_Images/Icons/Menu/about.png"), "About", this);

    helpMenu->addAction(aboutAction);

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
                cFile.completeBaseName() + "?",
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

    // Keep sideBar synchronized too.
    QModelIndex sideBarIndex = sideBarModel->index(path);
    sideBar->expand(sideBarIndex);
    sideBar->scrollTo(sideBarIndex, QAbstractItemView::PositionAtTop);
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

//void MainWindow::onCopyActionTriggered()
//{
////    QModelIndex cIndex = mainExplorer->currentIndex();
////    if(!cIndex.isValid()) return;

////    QString cPath = mainExplorerModel->fileInfo(cIndex);

////    QFile()
//}
