/****************************************************
 *
 * Author: Akela1101 <akela1101@gmail.com>
 *
 ****************************************************/

#include <QTextStream>

#include "Nya.hpp"
#include "ConfigNya.hpp"


namespace Nya
{
/**
 *  Загрузить из текста.
 */
void Config::Load(QTextStream& its, bool isMainConfig)
{
	for( int count = 0; !its.atEnd(); ++count )
	{
		QString key, s = its.readLine();
		if( !s.size() || s[0] == '#' )
		{
			key = s; // Запоминание строк по порядку.
		}
		else // Не пустая строка и не коммент
		{
			// # Формат:
			// Переменная = любые-символы-даже-через-пробел
			//
			QRegExp re("^\\s*(\\w+)\\s*=\\s*(\\S+.*)\\s*$");
			bool isValid = false;
			if( -1 != re.indexIn(s) )
			{
				QStringList keyValue = re.capturedTexts();
				if( keyValue.size() == 3 )
				{
					key = keyValue[1];

					// Добавить переменную. Если уже есть, заменить.					
					insert(key, keyValue[2]);
					isValid = true;
				}
			}
			else
			{
				QRegExp re("^\\s*(\\w+)\\s*=\\s*$");
				if( -1 != re.indexIn(s) )
				{
					QStringList keyValue = re.capturedTexts();
					if( keyValue.size() == 2 )
					{
						key = keyValue[1];
						insert(key, "");
						isValid = true;
					}
				}
			}
			if( !isValid )
			{
				l_warning << "Wrong string in config on line " << count << ": " << s;
			}
		}
		if( isMainConfig )
		{
			orderedKeys.push_back(key);
		}
	}
}

/**
 *  Сохранить как текст.
 */
void Config::Save(QTextStream& ots)
{
	for( const QString& key : orderedKeys )
	{
		ots << key;
		if( contains(key) )
		{
			ots << " = " << value(key);
		}
		ots << endl;
	}
}

}
