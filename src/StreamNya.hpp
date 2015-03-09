#ifndef STREAMNYA_H
#define STREAMNYA_H

#include <QByteArray>
#include <QHash>
#include <QVector>
#include <type_traits>
#include <limits>

#include "Nya.hpp"


/**
 * Data stream.
 */
namespace Nya
{
	class Stream
	{
		QIODevice* ioDevice;
		u_p<QDataStream> dataStream;

	public:
		Stream(QIODevice* ioDevice);
		~Stream();

		bool IsReadable() const;
		void SetFailWrite();
		void SetFailRead();
		operator bool() const; // Статус потока.

		// Удобные функции с указанием типа данных для размера массива.
		template<typename S>
		Stream& WriteArray(const QByteArray& ba)
		{
			if( (S)ba.size() > std::numeric_limits<S>::max() )
			{
				l_error << "Too long byte array will be cropped! [" << ba << "]";
			}
			S len = ba.size();
			*this << len;
			WriteRawData(ba.data(), len);
			return *this;
		}
		template<typename S>
		Stream& ReadArray(QByteArray& ba)
		{
			S len;
			*this >> len;
			ba.resize(len);
			ReadRawData(ba.data(), len);
			return *this;
		}

		// Удобная функция для чтения одного значения.
		template<typename T>
		T Read()
		{
			T value;
			*this >> value;
			return value;
		}

	private:
		// Две функции, через которые работают все остальные.
		void WriteRawData(const char* data, int len);
		void ReadRawData(char* data, int len);

		template<typename T>
		friend Stream& operator <<(Stream& stream, const T& t);
		template<typename T>
		friend Stream& operator >>(Stream& stream, T& t);
		friend Stream& operator <<(Stream& stream, const QByteArray& ba);
		friend Stream& operator >>(Stream& stream, QByteArray& ba);
	};
	Stream& operator <<(Stream& stream, const QByteArray& ba);
	Stream& operator >>(Stream& stream, QByteArray& ba);
	Stream& operator <<(Stream& stream, const QString& s);
	Stream& operator >>(Stream& stream, QString& s);
	Stream& operator <<(Stream& stream, const QStringList& sl);
	Stream& operator >>(Stream& stream, QStringList& sl);
	Stream& operator <<(Stream& stream, time_t t);
	Stream& operator >>(Stream& stream, time_t& t);
	Stream& operator <<(Stream& stream, bool b);
	Stream& operator >>(Stream& stream, bool& b);
	//
	Stream& operator <<(Stream& stream, const char* s);


	/**
	 * Несоставные переменные.
	 */
	template<typename T>
	Stream& operator <<(Stream& stream, const T& t)
	{
		static_assert(std::is_fundamental<T>::value, "Trying to put compound variable in Nya::Stream!");

		stream.WriteRawData((char*)&t, sizeof(T));
		return stream;
	}
	template<typename T>
	Stream& operator >>(Stream& stream, T& t)
	{
		static_assert(std::is_fundamental<T>::value, "Trying to put compound variable in Nya::Stream!");

		stream.ReadRawData((char*)&t, sizeof(T));
		return stream;
	}

	/**
	 * shared_ptr. Перед обработкой разыменовывает(!), а не берёт указатель.
	 */
	template<typename T>
	Stream& operator <<(Stream& stream, const s_p<T>& t)
	{
		return stream << *t;
	}
	template<typename T>
	Stream& operator >>(Stream& stream, s_p<T>& t)
	{
		if( !t ) t.reset(new T);
		return stream >> *t;
	}

	/**
	 * Список (размер 2 байта ~65 000 штук).
	 * Преимущественно для строковых переменных (не указателей, т.к. QList всё равно внутри их юзает).
	 */
	template<typename T>
	Stream& operator <<(Stream& stream, const QList<T>& l)
	{
		ushort n = l.size();
		stream << n;
		for( const T& e : l )
		{
			stream << e;
		}
		return stream;
	}
	template<typename T>
	Stream& operator >>(Stream& stream, QList<T>& l)
	{
		l.clear();
		ushort n = 0;
		stream >> n;
		l.reserve(n);
		for( ; n; --n )
		{
			T e;
			stream >> e;
			if( !stream )
			{
				l_error << "Unexpected end of message.";
				break;
			}
			l.push_back(e);
		}
		return stream;
	}

	/**
	 * Список (размер 4 байта ~2 000 000 000 штук).
	 * Для длинных массивов состояний (указатели или числа).
	 * QVector удобней юзать чем s_p<std::deque>.
	 */
	template<typename T>
	Stream& operator <<(Stream& stream, const QVector<T>& v)
	{
		int n = v.size();
		stream << n;
		for( const T& e : v )
		{
			stream << e;
		}
		return stream;
	}
	template<typename T>
	Stream& operator >>(Stream& stream, QVector<T>& v)
	{
		v.clear();
		int n = 0;
		stream >> n;
		v.resize(n);
		for( auto it = v.begin(); it != v.end(); ++it )
		{
			stream >> *it;
			if( !stream )
			{
				l_error << "Unexpected end of message.";
				break;
			}
		}
		return stream;
	}

	/**
	 * Хеш отображение строки (размер до ~65 000 штук).
	 */
	template<typename T>
	Stream& operator <<(Stream& stream, const QHash<QString, T>& h)
	{
		ushort n = h.size();
		stream << n;
		for( auto it = h.begin(); it != h.end(); ++it )
		{
			stream << it.key() << it.value();
		}
		return stream;
	}
	template<typename T>
	Stream& operator >>(Stream& stream, QHash<QString, T>& h)
	{
		h.clear();
		ushort n = 0;
		stream >> n;
		h.reserve(n);
		for( ; n; --n )
		{
			QString key;
			T value;
			stream >> key >> value;
			h.insert(key, value);
		}
		return stream;
	}
}

#endif // STREAMNYA_H
