#ifndef DIRNYA_H
#define DIRNYA_H

#include <QString>

namespace nya
{
QString MakeDirPath(const QString& dir);
bool MakeDirIfNone(const QString& dir);
}

#endif // DIRNYA_H
