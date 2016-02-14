#include <QFileInfo>
#include "guifile.h"
#include "filecopier.h"
#include "filedetail.h"

GuiFile::GuiFile(const QString &name) :
    QFile(name)
{
    // Nothing here
}

GuiFile::GuiFile(const QFile &file) :
    QFile(file.fileName()) // since copy constructor disabled
{
    // Nothing here
}

void GuiFile::guiDetail()
{
    FileDetail *detail = new FileDetail(this);
    detail->show();
}

void GuiFile::guiCopy(const QString &to)
{
    copyDest_ = to;

    FileCopier *copier = new FileCopier(0, this);

    QFileInfo info(*((QFile *)this));

    copier->copy(info.absoluteFilePath(), to);

    connect(copier, SIGNAL(copied()),
            copier, SLOT(deleteLater()));
    connect(copier, SIGNAL(error(FileCopier::ErrorType)),
            this, SLOT(emitCopyError(FileCopier::ErrorType)));
    connect(copier, SIGNAL(copied()),
            this, SLOT(emitFileCopied()));
}

void GuiFile::emitCopyError(FileCopier::ErrorType err)
{
    emit copyError(err, this, copyDest_);
}

void GuiFile::emitFileCopied()
{
    emit fileCopied(this);
}
