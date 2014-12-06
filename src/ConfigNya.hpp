/****************************************************
 *
 * Author: Akela1101 <akela1101@gmail.com>
 *
 ****************************************************/

#ifndef CONFIGNYA_H
#define CONFIGNYA_H

#include <QString>
#include <QHash>
#include <QVector>


class QTextStream;

namespace Nya
{
	class Config : public QHash<QString, QString>
	{
		QVector<QString> orderedKeys;

	public:
		void Load(QTextStream& its, bool isMainConfig = true);
		void Save(QTextStream& ots);
	};
}
#endif // CONFIGNYA_H
