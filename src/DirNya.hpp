/****************************************************
 *
 * Author: Akela1101 <akela1101@gmail.com>
 *
 ****************************************************/

#ifndef DIRNYA_H
#define DIRNYA_H

#include <QString>

namespace Nya
{
    QString MakeDirPath(const QString& dir);
    bool MakeDirIfNone(const QString& dir);
}

#endif // DIRNYA_H
