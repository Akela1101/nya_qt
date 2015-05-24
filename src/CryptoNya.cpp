#ifndef QT_NO_OPENSSL
#include <QString>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <openssl/aes.h>
#include <openssl/evp.h>

#include "CryptoNya.hpp"
#include "LogNya.hpp"


namespace Nya
{
static EVP_CIPHER_CTX cAES;
static SHA256_CTX c256;
static SHA512_CTX c512;


/**
 * Зашифровать в AES 256 CBC.
 */
QByteArray Crypto::Encrypt256(const QByteArray& a)
{
	QByteArray ret;
	int len = 0;
	int blockLen = 0;

	// Дополнить размер до AES_BLOCK_SIZE
	int mod = a.size() % AES_BLOCK_SIZE;
	ret.resize(mod ? a.size() + (AES_BLOCK_SIZE - mod) : a.size());

	EVP_CIPHER_CTX_init(&cAES);
	EVP_EncryptInit(&cAES, EVP_aes_256_cbc(), keyAES, ivAES);
	EVP_EncryptUpdate(&cAES, (uchar*)ret.data(), &len, (uchar*)a.data(), a.size());
	EVP_EncryptFinal(&cAES, (uchar*)ret.data() + len, &blockLen);
	EVP_CIPHER_CTX_cleanup(&cAES);

	ret.resize(len + blockLen);
	return ret;
}

/**
 * Расшифровать AES 256 CBC.
 */
QByteArray Crypto::Decrypt256(const QByteArray& b)
{
	QByteArray ret;
	int len = 0;
	int blockLen = 0;
	ret.resize(b.size());

	EVP_CIPHER_CTX_init(&cAES);
	EVP_DecryptInit(&cAES, EVP_aes_256_cbc(), keyAES, ivAES);
	EVP_DecryptUpdate(&cAES, (uchar*)ret.data(), &len, (uchar*)b.data(), b.size());
	EVP_DecryptFinal(&cAES, (uchar*)ret.data() + len, &blockLen);
	EVP_CIPHER_CTX_cleanup(&cAES);

	ret.resize(len + blockLen);
	return ret;
}

/**
 * 32 байта для хранения на клиенте.
 *
 * salt - любая постоянная последовательность байт.
 * Рэндомная соль не нужна, т.к. всё-равно один пароль.
 * Да и сама соль нужна только для страха, ибо её можно найти в коде.
 */
QByteArray Crypto::GetHash256(const QString& s, QByteArray& salt)
{
	QByteArray ret;
	ret.resize(SHA256_DIGEST_LENGTH);

	// Пароль = пароль + соль
	QByteArray sArray = s.toUtf8() + salt;

	// Хэш = sha256
	SHA256_Init(&c256);
	SHA256_Update(&c256, sArray.data(), sArray.size());
	SHA256_Final((uchar*)ret.data(), &c256);
	return ret;
}

/**
 * Вспомогательная функция. Вычисляет хэш и записывает его в массив после соли.
 */
static void GetHash512AfterSalt(const QByteArray& a, QByteArray& salt)
{
	// Пароль = пароль + соль
	QByteArray sArray = a;
	sArray.append(salt.data(), SHA512_DIGEST_LENGTH);

	// Хэш = соль + sha512
	SHA512_Init(&c512);
	SHA512_Update(&c512, sArray.data(), sArray.size());
	SHA512_Final((uchar*)salt.data() + SHA512_DIGEST_LENGTH, &c512);
}

/**
 * 64 + 64 байта на сервере.
 *
 * Соль нужна, чтобы нельзя было заюзать готовую радужную таблицу.
 * Рэндомность - чтобы нельзя было сделать радужную таблицу.
 */
QByteArray Crypto::GetHash512(const QByteArray& a)
{
	QByteArray ret;
	ret.resize(2 * SHA512_DIGEST_LENGTH);
	RAND_bytes((uchar*)ret.data(), SHA512_DIGEST_LENGTH); // salt

	GetHash512AfterSalt(a, ret);
	return ret;
}

/**
 * Проверка хэша.
 * Массив hash должен содержать соль и сам хэш.
 */
bool Crypto::CheckHash512(const QByteArray& a, const QByteArray& hash)
{
	if( hash.size() != 2 * SHA512_DIGEST_LENGTH )
	{
		l_error << "Wrong hash in database: [" << hash.size() << "] '" << hash << "' " << hash.toHex();
		return true;
	}
	QByteArray ret = hash;
	GetHash512AfterSalt(a, ret);
	return ret == hash;
}

}
#endif // QT_NO_OPENSSL
