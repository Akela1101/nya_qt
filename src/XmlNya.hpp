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
typedef QHash<QString, QStringRef> AttrsType;

namespace Nya
{
class XmlReader : public QXmlStreamReader
{
public:
	XmlReader(const char* data) : QXmlStreamReader(data) {}

	inline bool ReadNextChild() { return readNext() == TokenType::StartElement; }
	inline QString ReadElement() { return readElementText(QXmlStreamReader::IncludeChildElements); }
	inline void SkipElement() { readElementText(QXmlStreamReader::IncludeChildElements); }

	AttrsType GetAttributes();
};
}

#endif // XMLNYA_H
