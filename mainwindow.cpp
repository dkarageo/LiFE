#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->centralWidget->setWindowTitle("LiFE");

    // Creating the main layout of the centralWidget.
    QHBoxLayout *layout = new QHBoxLayout(this);

    // Creating the two main views of the UI
    sideBar = new QTreeView(this);
    mainExplorer = new QTableView(this);

    // Adding the two main views inside a splitter
    QSplitter *splitter = new QSplitter(this);
    splitter->addWidget(sideBar);
    splitter->addWidget(mainExplorer);

    layout->addWidget(splitter);

    // Setting initial values for the initial splitting
    QList<int> splittedSizes;
    splittedSizes << 300 << 700;
    splitter->setSizes(splittedSizes);

    sideBarModel = new QFileSystemModel(this);
    mainExplorerModel = new QFileSystemModel(this);

    // Making models to allow filesystem writing.
    sideBarModel->setReadOnly(false);
    mainExplorerModel->setReadOnly(false);

    QString initialRootPath = "C:/";

    // Creating data models for sideBar and mainExplorer
    sideBarModel->setRootPath(initialRootPath);
    sideBarModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);

    mainExplorerModel->setRootPath(initialRootPath);
    mainExplorerModel->setFilter(QDir::NoDotAndDotDot | QDir::AllEntries);

    // Adding models to views.
    sideBar->setModel(sideBarModel);
    mainExplorer->setModel(mainExplorerModel);

    ui->centralWidget->setLayout(layout);

    setupMenubar();
    setupToolbar();
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

void MainWindow::onSideBarClicked(const QModelIndex &index)
{
    // Getting path of folder whose contents mainExplorer should show.
    QString path = sideBarModel->fileInfo(index).absoluteFilePath();

    mainExplorer->setRootIndex(mainExplorerModel->setRootPath(path));
}

void MainWindow::onMainExplorerDoubleClicked(const QModelIndex &index)
{
    // Getting path of folder whose contents mainExplorer should show.
    QString path = mainExplorerModel->fileInfo(index).absoluteFilePath();

    mainExplorer->setRootIndex(mainExplorerModel->setRootPath(path));
}

void MainWindow::onMainExplorerRowCountChanged(int rowCount)
{

}

void MainWindow::onCopyActionTriggered()
{
    QModelIndex cIndex = mainExplorer->currentIndex();
    if(!cIndex.isValid()) return;

    QString cPath = mainExplorerModel->fileInfo(cIndex);

    QFile()
}

void MainWindow::setupMenubar()
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

// Help Menu Actions
    QAction *aboutAction = new QAction(
            QIcon(":/_Images/Icons/Menu/about.png"), "About", this);

    helpMenu->addAction(aboutAction);

// Adding Useful Actions to mainToolbar
    ui->mainToolBar->addAction(copyAction);
    ui->mainToolBar->addAction(pasteAction);
    ui->mainToolBar->addAction(deleteAction);
}

void MainWindow::setupToolbar()
{

}

void MainWindow::setupStatusbar()
{
    ui->statusBar->showMessage(QString::number(mainExplorerModel->rowCount()) + " objects");
}
