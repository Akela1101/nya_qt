/****************************************************
 *
 * Author: Akela1101 <akela1101@gmail.com>
 *
 ****************************************************/

#ifndef XMLNYA_H
#define XMLNYA_H

#include <QXmlStreamReader>
#include <QHash>


typedef QXmlStreamReader::TokenType XmlType;
typedef QHash<QString, QString> AttrsType;

namespace Nya
{
class XmlReader : public QXmlStreamReader
{
public:
	static void PrintHumanReadableXml(QIODevice* deviceOut, char* data);

	using QXmlStreamReader::QXmlStreamReader;

	bool ReadNextChild()
	{
		auto tt = readNext();
		if( tt == TokenType::Characters ) tt = readNext(); // skip '\n' and other trash
		return tt == TokenType::StartElement;
	}
	inline QString ReadElement() { return readElementText(QXmlStreamReader::IncludeChildElements); }
	inline void SkipElement() { readElementText(QXmlStreamReader::IncludeChildElements); }

	AttrsType GetAttributes();
};
}

#endif // XMLNYA_H
