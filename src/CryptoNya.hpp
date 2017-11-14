#ifndef CRYPTONYA_H
#define CRYPTONYA_H

#ifndef QT_NO_OPENSSL
#include <QByteArray>


namespace nya
{
class Crypto
{
	const uchar* keyAES;
	const uchar* ivAES;

public:
	Crypto(const uchar keyAES[32], const uchar ivAES[32]) : keyAES(keyAES), ivAES(ivAES) {}

	QByteArray Encrypt256(const QByteArray& a);
	QByteArray Decrypt256(const QByteArray& b);

	// Hash
	static QByteArray GetHash256(const QString& s, QByteArray& salt);
	static QByteArray GetHash512(const QByteArray& a);
	static bool CheckHash512(const QByteArray& a, const QByteArray& hash);
};
}
#endif // QT_NO_OPENSSL

#endif // CRYPTONYA_H
