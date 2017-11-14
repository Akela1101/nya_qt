#include <QFile>
#include <QStringList>

#include "ApplicationGuiNya.hpp"


namespace nya
{
/**
 * Соединить стили из нескольких файлов в один.
 */
QString LoadStyles(const QStringList& fileNames)
{
	QString ret;
	for (const QString& fileName : fileNames)
	{
		QFile qssFile(fileName);
		qssFile.open(QFile::ReadOnly);
		ret += qssFile.readAll();
		qssFile.close();
	}
	return ret;
}
}
