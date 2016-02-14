#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
#include "enhancedqt.h"
#include "guifile.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

// Configuring main windows's attributes
    this->setWindowTitle("LiFE");
    this->setWindowIcon(QIcon(":/_Images/Icons/Logo/logo.png"));

// Creating and configuring essential UI Widgets.    
    // Creating the main layout of the centralWidget.
    QHBoxLayout *layout = new QHBoxLayout(this);
    ui->centralWidget->setLayout(layout);

    // Creating the two main views of the UI
    sideBar = new QTreeView(this);
    sideBar->setSortingEnabled(true);
    sideBar->header()->hide();
    sideBar->setEditTriggers(QAbstractItemView::EditKeyPressed);

    mainExplorer = new QTableView(this);
    mainExplorer->setSortingEnabled(true);
    mainExplorer->verticalHeader()->hide();
    mainExplorer->setEditTriggers(QAbstractItemView::EditKeyPressed);
    mainExplorer->setContextMenuPolicy(Qt::CustomContextMenu);
    mainExplorer->setSelectionBehavior(QAbstractItemView::SelectRows);
    mainExplorer->setSelectionMode(QAbstractItemView::ExtendedSelection);

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

    // Adding models to views
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

// Initializing paste tools
    errors_ = new QLinkedList<UnhandledCopyError>;
    yestoall_ = false;
    notoall_ = false;
    existsDialogOpen_ = false;
    errorsAlreadyHandled_ = false;

// SIGNAL-SLOT connections go here
    connect(sideBar, SIGNAL(clicked(QModelIndex)),
            this, SLOT(onSideBarClicked(QModelIndex)));
    connect(mainExplorer, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(onMainExplorerDoubleClicked(QModelIndex)));
    connect(this, SIGNAL(mainExplorerRootIndexChanged(QModelIndex)),
            this, SLOT(setProperMainExplorerCurrentIndex()));
    connect(this, SIGNAL(mainExplorerRootIndexChanged(QModelIndex)),
            this, SLOT(updateWindowTitle()));
    connect(mainExplorer, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(popupContextMenu(QPoint)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::isCurrentDirEmpty()
{
    QDir dir(currentFolderAbsolutePath());

    return dir.count() > 0 ? true : false;
}

QString MainWindow::currentFolderAbsolutePath()
{
    QModelIndex cIndex = mainExplorer->currentIndex();

    return mainExplorerModel->fileInfo(cIndex).absolutePath();
}

QString MainWindow::currentItemAbsolutePath()
{
    QModelIndex cIndex = mainExplorer->currentIndex();

    return mainExplorerModel->fileInfo(cIndex).absoluteFilePath();
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
    goUp = new QAction(
            QIcon(":/_Images/Icons/Menu/upper_directory.png"), "Up", this);
    goUp->setShortcut(QKeySequence(QKeySequence::MoveToPreviousPage));

    exitAction = new QAction(
            QIcon(":/_Images/Icons/Menu/exit.png"), "Exit", this);
    exitAction->setShortcut(QKeySequence(QKeySequence::Close));

    fileMenu->addAction(goUp);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    connect(goUp, SIGNAL(triggered(bool)),
            this, SLOT(onGoUpActionTriggered()));
    connect(exitAction, SIGNAL(triggered(bool)), this, SLOT(close()));

// Edit Menu Actions
    copyAction = new QAction(
            QIcon(":/_Images/Icons/Menu/copy.png"), "Copy", this);
    copyAction->setShortcut(QKeySequence(QKeySequence::Copy));

    pasteAction = new QAction(
            QIcon(":/_Images/Icons/Menu/paste.png"), "Paste", this);
    pasteAction->setShortcut(QKeySequence(QKeySequence::Paste));

    deleteAction = new QAction(
            QIcon(":/_Images/Icons/Menu/delete.png"), "Delete", this);
    deleteAction->setShortcut(QKeySequence(QKeySequence::Delete));

    detailAction = new QAction(
            QIcon(":/_Images/Icons/Menu/properties.png"), "Properties", this);

    editMenu->addAction(copyAction);
    editMenu->addAction(pasteAction);
    editMenu->addAction(deleteAction);
    editMenu->addSeparator();
    editMenu->addAction(detailAction);

    connect(copyAction, SIGNAL(triggered(bool)),
            this, SLOT(onCopyActionTriggered()));
    connect(pasteAction, SIGNAL(triggered(bool)),
            this, SLOT(onPasteActionTriggered()));
    connect(deleteAction, SIGNAL(triggered(bool)),
            this, SLOT(onDeleteActionTriggered()));
    connect(detailAction, SIGNAL(triggered(bool)),
            this, SLOT(onDetailActionTriggered()));

// Help Menu Actions
    QAction *aboutAction = new QAction(
            QIcon(":/_Images/Icons/Menu/about.png"), "About", this);
    aboutAction->setShortcut(QKeySequence(QKeySequence::HelpContents));

    helpMenu->addAction(aboutAction);

    connect(aboutAction, SIGNAL(triggered(bool)),
            this, SLOT(onAboutActionTriggered()));

// Other Actions
    openAction = new QAction("Open", this);
    QFont font = openAction->font();
    font.setBold(true);
    openAction->setFont(font);

    newFolderAction = new QAction(
                QIcon(":/_Images/Icons/Menu/new_folder.png"), "New Folder", this);
    newFolderAction->setShortcut(QKeySequence(QKeySequence::New));

    connect(openAction, SIGNAL(triggered(bool)),
            this, SLOT(onOpenActionTriggered()));
    connect(newFolderAction, SIGNAL(triggered(bool)),
            this, SLOT(onNewFolderAction()));

// Adding Useful Actions to mainToolbar
    ui->mainToolBar->addAction(goUp);
    ui->mainToolBar->addAction(copyAction);
    ui->mainToolBar->addAction(pasteAction);
    ui->mainToolBar->addAction(deleteAction);
    ui->mainToolBar->addAction(newFolderAction);
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
    QModelIndexList cIndexes = mainExplorer->selectionModel()->selectedRows();
    QList<QUrl> urlsList;

    foreach(QModelIndex cIndex, cIndexes) {
        // Get path of file to be copied.
        QString cPath = mainExplorerModel->fileInfo(cIndex).absoluteFilePath();
        urlsList.append(QUrl::fromLocalFile(cPath));
    }

    // Create QMimeData object, that works on Windows and wherever lucky!
    QMimeData *data = new QMimeData();
    data->setUrls(urlsList);

    clipboard->setMimeData(data);
}

void MainWindow::onDeleteActionTriggered()
{
    QModelIndexList indexes = mainExplorer->selectionModel()->selectedRows();

    QListIterator<QModelIndex> iter(indexes);
    QFileInfoList fInfos;

    // From selected indexes, create a QFileInfoList only
    // containing files that can be deleted.
    while(iter.hasNext()) {
        QModelIndex cIndex = iter.next();

        // Check further only for valid indexes.
        if(cIndex.isValid()) {
            QFileInfo info = mainExplorerModel->fileInfo(cIndex);

            // And the actuall check on a valid index.
            if((info.isDir() | info.isFile()) && !info.isRoot() && info.isWritable()) {
                fInfos.append(info);
            }
        }
    }

    int fCount = fInfos.count();
    if (fCount == 0) return; // Nothing to delete, so don't continue.

    QString promptText; // Text to be displayed on prompt dialog.

    if(fCount == 1) { // Single file - display its name
        QFileInfo info = fInfos.first();

        promptText = "Do you want to permanently remove " +
                      info.completeBaseName();

        // Also append suffix, if any.
        QString suffix = info.completeSuffix();
        promptText += suffix.isEmpty() ? "?" : "." + suffix + "?";
    }
    else { // Multiple files - display their number
        promptText = "Do you want to permanently remove " +
                     QString::number(fCount) + " items?";
    }

    // Ask user about deletion.
    QMessageBox::StandardButton choice;
    choice = QMessageBox::question(this, "Confirm Deletion", promptText,
                                   QMessageBox::No | QMessageBox::Yes);

    // Simply return if user selects No.
    if(choice == QMessageBox::No) return;

    int fails = 0;

    // Actually delete 'em all!
    foreach(QFileInfo info, fInfos) {
        if (!threadsafeFileRemove(info.absoluteFilePath())) fails++;
    }

    // On fails display a message on statusBar.
    if(fails) {
        ui->statusBar->showMessage(QString::number(fails) + " files failed to be deleted.");
    }
}

void MainWindow::onDetailActionTriggered()
{
    GuiFile file(currentItemAbsolutePath());
    file.guiDetail();
}

void MainWindow::onGoUpActionTriggered()
{
    QModelIndex cIndex = mainExplorer->currentIndex();
    // If inside an empty dir, index should be invalid.
    // So return only when an index is invalid and
    // current dir is not empty.
    if(!cIndex.isValid() && !isCurrentDirEmpty()) return;

    // Using QDir to go to above dir, instead of unsafely using
    // QModelIndexes.
    QDir cDir(mainExplorerModel->rootPath());

    cDir.cdUp();
    QString upperPath = cDir.absolutePath();

    // Setting correct rootIndex for mainExplorer.
    QModelIndex mainExplorerIndex = mainExplorerModel->setRootPath(upperPath);
    mainExplorer->setRootIndex(mainExplorerIndex);
    emit mainExplorerRootIndexChanged(mainExplorerIndex);

    // Synchronizing statusBar.
    QModelIndex sideBarIndex = sideBarModel->index(upperPath);
    sideBar->setCurrentIndex(sideBarIndex);
    sideBar->scrollTo(sideBarIndex);
}

void MainWindow::onMainExplorerDoubleClicked(const QModelIndex &index)
{
    if(mainExplorerModel->fileInfo(index).isDir()) {
        // Getting path of folder whose contents mainExplorer should show.
        QString path = mainExplorerModel->fileInfo(index).absoluteFilePath();

        mainExplorer->setRootIndex(mainExplorerModel->setRootPath(path));
        emit mainExplorerRootIndexChanged(mainExplorer->rootIndex());

        // Keep sideBar synchronized too.
        QModelIndex sideBarIndex = sideBarModel->index(path);
        sideBar->setCurrentIndex(sideBarIndex);
        sideBar->expand(sideBarIndex);
        sideBar->scrollTo(sideBarIndex, QAbstractItemView::PositionAtTop);
    }
    else if(mainExplorerModel->fileInfo(index).isFile()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(
                                      mainExplorerModel->filePath(index)
        ));
    }
}

void MainWindow::onNewFolderAction()
{
    QDir cDir(mainExplorerModel->rootPath());

    QString newName = "New Folder";

    int i = 2;

    while(QFile(mainExplorerModel->rootPath() + '/' + newName).exists())
    {
        newName = "New Folder " + QString::number(i);
        ++i;
    }

    cDir.mkdir(newName);

    mainExplorer->setCurrentIndex(
                mainExplorerModel->index(
                    mainExplorerModel->rootPath() + '/' + newName
    ));

    mainExplorer->edit(mainExplorer->currentIndex());
}

void MainWindow::onOpenActionTriggered()
{
    QString cPath = currentItemAbsolutePath();

    if(QFileInfo(currentItemAbsolutePath()).isDir()) {
        // Open folders using the same mechanism as double clicking them.
        onMainExplorerDoubleClicked(mainExplorer->currentIndex());
    }
    else {
        QDesktopServices::openUrl(
                    QUrl::fromLocalFile(cPath));
    }

}

void MainWindow::onPasteActionTriggered()
{
    // Get the urls list from clipboard, if any.
    if(!clipboard->mimeData()->hasUrls()) return;

    QList<QUrl> urls = clipboard->mimeData()->urls();

    // Create destination path.
    QString destPath = mainExplorerModel->rootPath() + "/";

    foreach(QUrl url, urls) {
        QString fPath = url.toLocalFile(); // Translate url to absolute path.
        QFileInfo fileInfo(url.toLocalFile());

        QString cFinalPath;

        // Create the final absolute path, where fPath file will be copied.
        if(fileInfo.isDir()) {
            cFinalPath = destPath + fileInfo.completeBaseName();
        }
        else {
            cFinalPath = destPath + fileInfo.completeBaseName() +
                         "." + fileInfo.completeSuffix();
        }

        GuiFile *copier = new GuiFile(fPath);
        copier->guiCopy(cFinalPath);

        connect(copier, SIGNAL(fileCopied(GuiFile *)),
                copier, SLOT(deleteLater()));
        connect(copier, SIGNAL(copyError(FileCopier::ErrorType, GuiFile *, const QString &)),
                this, SLOT(handlePasteError(FileCopier::ErrorType, GuiFile *, const QString &)));
    }
}

void MainWindow::handlePasteError(FileCopier::ErrorType err, GuiFile *file,
                                  const QString &to)
{
    switch(err) {
        case FileCopier::Exists:
            if(yestoall_) {
                // "YesToAll" selected, so don't notify user anymore.
                if(threadsafeFileRemove(to)) {
                    file->guiCopy(to);
                }
            }
            else if(notoall_) {
                // "NoToAll" selected, so just do nothing for this file.
            }
            else {
                // No "YesToAll" selected, so ask user what to do.

                QFileInfo fileInfo(*((QFile *)file));

                // A simple mechanism to queue exist error handlings so only one dialog
                // is opened at a time, and if a mass prompt given, no other is opened.
                if(existsDialogOpen_) {
                    UnhandledCopyError curErr = {err, file, to};
                    errors_->append(curErr);
                    return;
                }

                // Indicates that a dialog asking about Exist error is opened.
                // So next errors, are gonna be queued.
                existsDialogOpen_ = true;

                QMessageBox::StandardButton choice = QMessageBox::question(
                        this, "How to copy file?",
                        "File \"" + fileInfo.completeBaseName() +
                        "." + fileInfo.completeSuffix() +
                        "\" already exists. Do you want to overwrite it?",
                        QMessageBox::No | QMessageBox::NoToAll | QMessageBox::Yes | QMessageBox::YesToAll
                );

                // Dialog closed.
                existsDialogOpen_ = false;

                if(choice == QMessageBox::Yes) {
                    if(threadsafeFileRemove(to)) {
                        file->guiCopy(to);
                    }
                }
                else if(choice == QMessageBox::YesToAll) {
                    yestoall_ = true;

                    if(threadsafeFileRemove(to)) {
                        file->guiCopy(to);
                    }
                }
                else if(choice == QMessageBox::NoToAll) {
                    notoall_ = true;
                }
                // "No" or anything else - nothing to do, so no else
            }
        break;

        default: // Just do nothing on other errors.
            return;
    }

    if(!existsDialogOpen_ && !errorsAlreadyHandled_) {
        errorsAlreadyHandled_ = true;

        while(errors_->count() > 0) {
            UnhandledCopyError uErr = errors_->first();

            handlePasteError(uErr.err, uErr.file, uErr.to);

            errors_->removeFirst();
        }

        errorsAlreadyHandled_ = false;
        yestoall_ = false;
        notoall_ = false;
    }
}

void MainWindow::onSideBarClicked(const QModelIndex &index)
{
    // Getting path of folder whose contents mainExplorer should show.
    QString path = sideBarModel->fileInfo(index).absoluteFilePath();

    mainExplorer->setRootIndex(mainExplorerModel->setRootPath(path));
    emit mainExplorerRootIndexChanged(mainExplorer->rootIndex());
}

void MainWindow::popupContextMenu(const QPoint &point)
{
    QModelIndex index = mainExplorer->indexAt(point);

    QMenu *contextMenu = new QMenu(this);
    QList<QAction *> actions;

    if(index != QModelIndex() &&
       !mainExplorerModel->fileInfo(index).isRoot())
    {
        // index is valid - handle a proper file/dir
        actions << openAction;
        actions << contextMenu->addSeparator();
        actions << copyAction;
        actions << deleteAction;
        actions << contextMenu->addSeparator();
        actions << detailAction;
    }
    else {
        // index is invalid - provide generic context menu
        actions << pasteAction;
        actions << contextMenu->addSeparator();
        actions << newFolderAction;
    }

    contextMenu->addActions(actions);
    contextMenu->popup(mainExplorer->viewport()->mapToGlobal(point));
}

void MainWindow::refreshStatusBarCounter()
{
    // Getting directory set as root directory in mainExplorerModel.
    QDir cDir = mainExplorerModel->rootDirectory();
    cDir.setFilter(QDir::NoDotAndDotDot | QDir::AllEntries);

    ui->statusBar->showMessage(QString::number(cDir.count()) + " objects");
}

void MainWindow::setProperMainExplorerCurrentIndex()
{
    QDir cRootDir = mainExplorerModel->rootDirectory();

    if(cRootDir.count() > 0) {
        mainExplorer->setCurrentIndex(
                mainExplorerModel->index(
                        cRootDir.entryInfoList().first().absoluteFilePath(),
                        QItemSelectionModel::NoUpdate
                )
        );
    }
    else {
        mainExplorer->setCurrentIndex(QModelIndex());
    }
}

void MainWindow::updateWindowTitle()
{
    setWindowTitle(mainExplorerModel->rootPath() + " - LiFE");
}
