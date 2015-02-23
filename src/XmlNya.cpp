/****************************************************
 *
 * Copyright (c) 2014 Akela1101 <akela1101@gmail.com>
 *
 ****************************************************/

#include <QTextStream>

#include "XmlNya.hpp"


namespace Nya
{
/**
 * Convert to formatted xml.
 */
QString XmlReader::ToFormatted(char* data)
{
	QString ret;
	QXmlStreamWriter ssOut(&ret);
	ssOut.setAutoFormatting(true);

	Nya::XmlReader ssIn(data);
	while( ssIn.readNext() > 1 ) ssOut.writeCurrentToken(ssIn);
	return ret;
}

/**
 * Human readable log.
 */
QString XmlReader::ToHumanReadable(char* data)
{
	QString ret;
	QTextStream ssOut(&ret);

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
	return ret;
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
}
