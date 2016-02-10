#include "guifile.h"
#include "filedetail.h"

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
