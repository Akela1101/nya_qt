#ifndef CRYPTONYA_H
#define CRYPTONYA_H

#include <QByteArray>


namespace Nya
{
	struct Crypto
	{
		// Hash
		static QByteArray GetHash256(const QString& s);
		static QByteArray GetHash512(const QByteArray& a);
		static bool CheckHash512(const QByteArray& a, const QByteArray& hash);

		// Encrypt
		static QByteArray Encrypt256(const QByteArray& a);
		static QByteArray Decrypt256(const QByteArray& b);
	};
}

#endif // CRYPTONYA_H
