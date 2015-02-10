/****************************************************
 *
 * Author: Akela1101 <akela1101@gmail.com>
 *
 ****************************************************/

#include <QTextStream>

#include "XmlNya.hpp"


using namespace Nya;


/**
 * Human readable log.
 */
void XmlReader::PrintHumanReadableXml(QIODevice* deviceOut, char* data)
{
	QTextStream ssOut(deviceOut);
	QXmlStreamReader ssIn(data);
	QString tabs;
	while( !ssIn.atEnd() )
	{
		XmlType tt = ssIn.readNext();
		if( tt == XmlType::StartElement )
		{
			tabs += "   ";
			ssOut << QString(tabs).append(ssIn.name());
			for( auto& attr : ssIn.attributes() )
			{
				ssOut << QString(" ").append(attr.name()).append(" = ").append(attr.value());
			}
			ssOut << "\n";
		}
		else if( tt == XmlType::Characters )
		{
			tabs += "   ";
			ssOut << QString(tabs).append(ssIn.text()) << "\n";
		}
		if( tt == XmlType::EndElement || tt == XmlType::Characters ) tabs.chop(3);
	}
}

/**
 * Get attributes.
 */
AttrsType XmlReader::GetAttributes()
{
	AttrsType attrs;
	QXmlStreamAttributes as = attributes();
	for( auto& a : as ) attrs.insert(a.name().toString(), a.value().toString());
	return attrs;
}
