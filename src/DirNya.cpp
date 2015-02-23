/****************************************************
 *
 * Copyright (c) 2013 Akela1101 <akela1101@gmail.com>
 *
 ****************************************************/

#include "Nya.hpp"

#include <QDir>

#include "DirNya.hpp"


namespace Nya
{
QString MakeDirPath(const QString& dir)
{
	return !dir.size() ? "./" : (dir[dir.size()-1] == '/') ? dir : dir + "/";
}
bool MakeDirIfNone(const QString& dir)
{
	bool ret = QDir(dir).exists() || QDir().mkpath(dir);
	if( !ret ) l_info << "Unable to create directory in " << dir;
	return ret;
}
}
