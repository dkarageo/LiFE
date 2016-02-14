#ifndef ENHANCEDQT_H
#define ENHANCEDQT_H

#include <QThread>
#include <QDir>
#include <QFile>
#include <QFileInfo>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Qt's QFile::remove() or QDir::removeRecursively() doesn't take
 * care the case where a file removed by them is immediately created
 * by another thread.
 *
 * threadsafeFileRemove() is actually doing a three times remove
 * of the file/dir to decrease the possibilities of this to happen.
 * Though the extreme case of a file being always regenerated is
 * not handled.
 *
 * It removes files using QFile::remove() and dirs using
 * QDir::removeRecursively() making it the all-in-one deleter.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */
bool threadsafeFileRemove(const QString &path)
{
    QFile file(path);

    bool removed = false;

    for(int i = 0; i < 3 && file.exists(); ++i) {
        if(removed && i > 0) {
        // File was removed once, and is here again.
            QThread::msleep(5); // Wait 5ms now,
                                // and then repeat operation.
        }

        QFileInfo info(file);

        if(info.isDir()) {
            QDir dir(info.absoluteFilePath());
            removed = dir.removeRecursively();
        }
        else {
            removed = file.remove();
        }
    }

    return !file.exists();
}

#endif // ENHANCEDQT_H

