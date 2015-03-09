/****************************************************
 *
 * Copyright (c) 2015 Akela1101 <akela1101@gmail.com>
 *
 ****************************************************/

#ifndef LOGNYA_H
#define LOGNYA_H

/*
Example:
{
	NyaLog.AddLogger(Nya::TRACE); // console
	NyaLog.AddLogger(Nya::INFO, "info.log"); // file

	l_trace << QString::fromUtf8("Первая строка.");
	auto log = l_info << QString::fromUtf8("Вторая строка: ");
	log << QString::fromUtf8("состоит из двух.");
}
*/

#include "EnumNya.hpp"
#include "SingletonNya.hpp"
#include "Nya.hpp"

#include <QString>
#include <QVariant>
#include <QVector>


class QTextCodec;

namespace Nya
{
#undef TRACE
#undef INFO
#undef ERROR
#undef FAIL
#define LogLevelDef(K, L, M, N) \
	M(TRACE,    "Trace, Debug") \
	M(INFO,     "Info, Warnings") \
	M(ERROR,    "Program errors") \
	M(FAIL,     "Critical, Fatal")
NYA_ENUM(LogLevel, LogLevelDef)


// ~~~
class Logger
{
public:
	LogLevel level;
protected:
	QTextCodec* codec;

public:
	Logger(LogLevel level);
	void SetOutputCodec(const QByteArray& outputCodec);

	virtual void Write(const QString& message) = 0;
};

// ~~~
class Log : public Singleton<Log>
{
	QString timeFormat = NYA_TIME_FORMAT;       /// yyyy-MM-dd hh:mm:ss
	QString messageFormat = "[%1] %2 [%3:%4] "; /// time level file line
	QVector<s_p<Logger>> loggers;               /// loggers list

public:
	Log();
	QString GetTimeFormat() const { return timeFormat; }
	QString GetMessageFormat() const { return messageFormat; }
	void SetTimeFormat(const QString& timeFormat) { this->timeFormat = timeFormat; }
	void SetMessageFormat(const QString& messageFormat) { this->messageFormat = messageFormat; }

	Logger& AddLogger(LogLevel level);
	Logger& AddLogger(LogLevel level, const QString& filePath, bool isRewrite = false);

	void WriteAll(const QString& message, LogLevel level = TRACE);
};

// ~~~
class LogStreamShared;
class LogStream
{
	s_p<LogStreamShared> shared; // for logging from multiple lines

public:
	LogStream(LogLevel level, const char* file = 0, int line = 0);
	LogStream(const LogStream& o) : shared(o.shared) {}

	LogStream& operator<< (const QVariant& value);
};
}

#define NyaLog  Nya::Log::GS()
// with time, file, line:
#define l_trace Nya::LogStream(Nya::TRACE, __FILE__, __LINE__)
#define l_info  Nya::LogStream(Nya::INFO,  __FILE__, __LINE__)
#define l_error Nya::LogStream(Nya::ERROR, __FILE__, __LINE__)
#define l_fail  Nya::LogStream(Nya::FAIL,  __FILE__, __LINE__)
// without:
#define o_trace Nya::LogStream(Nya::TRACE)
#define o_info  Nya::LogStream(Nya::INFO)
#define o_error Nya::LogStream(Nya::ERROR)
#define o_fail  Nya::LogStream(Nya::FAIL)


#endif // LOGNYA_H
