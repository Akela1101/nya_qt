/****************************************************
 *
 * Author: Akela1101 <akela1101@gmail.com>
 *
 ****************************************************/

#include "XmlNya.hpp"

using namespace Nya;


AttrsType XmlReader::GetAttributes()
{
	AttrsType attrs;
	QXmlStreamAttributes as = attributes();
	for( auto& a : as ) attrs.insert(a.name().toString(), a.value().toString());
	return attrs;
}
