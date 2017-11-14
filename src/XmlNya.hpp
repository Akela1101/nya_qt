#ifndef XMLNYA_H
#define XMLNYA_H

/*
Usage:
	while( ss.ReadNextChild() )
	{
		QStringRef s = ss.name();
		if( s == "a1" ) a1 = ss.ReadElement().toLatin1();
		else if( s == "a2" ) a2 = ss.ReadElement().toDouble();
		// ...
		else ss.SkipElement();
	}
*/

#include <QXmlStreamReader>
#include <QHash>


typedef QXmlStreamReader::TokenType XmlType;
typedef QHash<QString, QString> AttrsType;

namespace nya
{
class XmlReader : public QXmlStreamReader
{
public:
	static QString ToFormatted(char* data);
	static QString ToHumanReadable(char* data);
	
	using QXmlStreamReader::QXmlStreamReader;
	
	bool ReadNextChild()
	{
		auto tt = readNext();
		if (tt == TokenType::Characters) tt = readNext(); // skip '\n' and other trash
		return tt == TokenType::StartElement;
	}
	inline QString ReadElement() { return readElementText(QXmlStreamReader::IncludeChildElements); }
	inline void SkipElement() { readElementText(QXmlStreamReader::IncludeChildElements); }
	
	AttrsType GetAttributes();
};
}

#endif // XMLNYA_H
