#include "NyaQt.hpp"

#include <QDir>

#include "DirNya.hpp"


namespace nya
{
QString MakeDirPath(const QString& dir)
{
	return !dir.size() ? "./" : (dir[dir.size() - 1] == '/') ? dir : dir + "/";
}
bool MakeDirIfNone(const QString& dir)
{
	bool ret = QDir(dir).exists() || QDir().mkpath(dir);
	if (!ret) info_log << "Unable to create directory in " << dir;
	return ret;
}
}
