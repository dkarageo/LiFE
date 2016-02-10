#ifndef GUIFILE_H
#define GUIFILE_H

class CopyStatus;
class FileDetail;

#include <QFile>
#include <QObject>

/*
 * GuiFile provides non-locking GUI representation for
 * common tasks done in QFile, like opening up
 * a progress dialog when copying a file or
 * providing a detail window for files.
 */
class GuiFile : public QFile
{
    Q_OBJECT
public:
    GuiFile(const QString &name);
    GuiFile(const GuiFile &file);

    //void guiCopy(const QString &to);
    //void guiCopy(const QString &from, const QString &to) const;

    void guiDetail();
};

#endif // GUIFILE_H
