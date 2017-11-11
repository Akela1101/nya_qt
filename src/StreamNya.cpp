#include <QIODevice>
#include <QDataStream>
#include <QStringList>

#include "StreamNya.hpp"


namespace Nya
{
static const int readTimeout = 72; // sec


Stream::Stream(QIODevice* ioDevice)
	: ioDevice(ioDevice)
	, dataStream(new QDataStream(ioDevice))
{
	dataStream->setByteOrder(QDataStream::LittleEndian);
	dataStream->setStatus(QDataStream::Ok);
}

Stream::~Stream() {}

/**
 * Есть ли данные для чтения.
 */
bool Stream::IsReadable() const
{
	return ioDevice->bytesAvailable();
}

/**
 * Установить статус потока невалидным.
 */
void Stream::SetFailWrite()
{
	dataStream->setStatus(QDataStream::WriteFailed);
}
void Stream::SetFailRead()
{
	dataStream->setStatus(QDataStream::ReadCorruptData);
}

/**
 * Статус потока.
 */
Stream::operator bool() const
{
	return dataStream->status() == QDataStream::Ok;
}

/**
 * Писать.
 */
void Stream::WriteRawData(const char* data, int len)
{
	int lenWrite = 0;
	while( lenWrite < len && *this )
	{
		lenWrite += dataStream->writeRawData(data, len);
	}
	if( lenWrite != len )
	{
		QByteArray dataBeginning(data, std::min(50, len));
		l_error << "Nya::Stream write fail: (" << len << ")[" << dataBeginning << "] But was written: (" << lenWrite << ")";
		SetFailWrite();
	}
}

/**
 * Читать.
 */
void Stream::ReadRawData(char* data, int len)
{
	int lenRead = 0;
	while( lenRead < len && (ioDevice->bytesAvailable() || ioDevice->waitForReadyRead(readTimeout * 1000)) )
	{
		lenRead += dataStream->readRawData(data + lenRead, len - lenRead);
	}
	if( lenRead != len )
	{
		QByteArray dataBeginning(data, std::min(50, lenRead));
		l_error << "Nya::Stream read fail: (" << len << ")[" << dataBeginning << "] But was read: (" << lenRead << ")";
		SetFailRead();
	}
}

//===============================================================================

/**
 * QByteArray - размер 1 байт. Если нужно больше, то руками писать.
 */
Stream& operator <<(Stream& stream, const QByteArray& ba)
{
	return stream.WriteArray<uchar>(ba);
}
Stream& operator >>(Stream& stream, QByteArray& ba)
{
	return stream.ReadArray<uchar>(ba);
}

/**
 * Строка Utf8 без нуля в конце [1]{абв}.
 */
Stream& operator <<(Stream& stream, const QString& s)
{
	const QByteArray ba = s.toUtf8();
	stream << ba;
	return stream;
}
Stream& operator >>(Stream& stream, QString& s)
{
	QByteArray ba;
	stream >> ba;
	s = QString::fromUtf8(ba.data(), ba.size());
	return stream;
}

/**
  * Список строк (размер 2 байта ~ 65 000 штук).
  */
Stream& operator <<(Stream& stream, const QStringList& l)
{
	return stream << (QList<QString>&)l;
}
Stream& operator >>(Stream& stream, QStringList& l)
{
	return stream >> (QList<QString>&)l;
}

/**
  * Время (4 байта даже для 64-битных систем).
  */
Stream& operator <<(Stream& stream, time_t t)
{
	stream << (int)t;
	return stream;
}
Stream& operator >>(Stream& stream, time_t& t)
{
	int iT;
	stream >> iT;
	t = (time_t)iT;
	return stream;
}

/**
 * Бул (1 байт).
 */
Stream& operator <<(Stream& stream, bool b)
{
	stream << (uchar)b;
	return stream;
}
Stream& operator >>(Stream& stream, bool& b)
{
	uchar u;
	stream >> u;
	b = (bool)u;
	return stream;
}

/**
 * C-строка.
 */
Stream& operator <<(Stream& stream, const char* s)
{
	return stream << QString(s);
}
}
