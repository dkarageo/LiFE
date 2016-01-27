#ifndef ENHANCEDQT_H
#define ENHANCEDQT_H

#include <QThread>
#include <QFile>

/*
 * Qt's QFile::remove() doesn't take care the case
 * where a file removed by it is immediately created
 * by another thread. threadsafeFileRemove() is
 * actually doing a three times remove of the file
 * to decrease the possibilities of this to happen.
 * Though the extreme case of a file being always
 * regenerated is not handled.
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

        removed = file.remove();
    }

    return !file.exists();
}

#endif // ENHANCEDQT_H

