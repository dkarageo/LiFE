#ifndef GUIFILE_H
#define GUIFILE_H

class CopyStatus;
class FileDetail;

#include <QFile>
#include <QObject>
#include "filecopier.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * GuiFile provides non-locking GUI representation for
 * common tasks done in QFile.
 *
 * GuiFile due to its async nature, should be allocated in heap.
 *
 * Task done by GuiFile are:
 *
 * - Providing a detail/properties window for files through
 *   guiDetail().
 *
 * - Opening up a progress dialog when copying a file/folder
 *   through guiCopy().
 *
 *   On file/folder copying if an error like it exists, or
 *   destination is not writable etc is encountered,
 *   copyError(FileCopier::ErrorType) signal is emitted
 *   and should be handled appropriately.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */
class GuiFile : public QFile
{
    Q_OBJECT
public:
    // Construct GuiFile by path.
    GuiFile(const QString &name);
    // Construct GuiFile by a QFile.
    GuiFile(const QFile &file);

    // Copies current file, to destination given.
    void guiCopy(const QString &to);

    // Opends up a FileDetail dialog for the given file.
    void guiDetail();

signals:
    // A signal emitted when an error ocurred during file copying.
    void copyError(FileCopier::ErrorType, GuiFile *, const QString &to);
    void fileCopied(GuiFile *);

private:
    // Keeps tracks of copy destination, for use in error signals.
    QString copyDest_;

private slots:
    // Used for passing signals from FileCopier to this.
    void emitCopyError(FileCopier::ErrorType err);
    void emitFileCopied();
};

#endif // GUIFILE_H
