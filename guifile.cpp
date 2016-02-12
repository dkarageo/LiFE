#include "guifile.h"
#include "filecopier.h"
#include "filedetail.h"

GuiFile::GuiFile()
{

}

GuiFile::GuiFile(const QString &name) :
    QFile(name)
{
    // Nothing here
}

GuiFile::GuiFile(const GuiFile &file) :
    QFile(file.fileName())
{
    // Nothing here
}

void GuiFile::guiDetail()
{
    FileDetail *detail = new FileDetail(this);
    detail->show();
}

bool GuiFile::guiCopy(const QString &from, const QString &to) const
{
    FileCopier *copier = new FileCopier(0);
    copier->copy(from, to);

    return true;
}
