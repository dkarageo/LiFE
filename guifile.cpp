#include "guifile.h"
#include "filedetail.h"

GuiFile::GuiFile(const QString &name) :
    QFile(name)
{

}

//GuiFile::GuiFile(const QFile &file) :
//    QFile(file)
//{

//}

GuiFile::GuiFile(const GuiFile &file) :
    QFile(file.fileName())
{

}

void GuiFile::guiDetail()
{
    FileDetail *detail = new FileDetail(this);
    detail->show();
}
