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
	NyaLog.SetTimeFormat("yyyy-MM-dd hh:mm:ss.zzz");
	NyaLog.SetMessageFormat("[%1] %2 [%3:%4] ");

	NyaLog.AddLogger(Nya::TRACE); // console
	NyaLog.AddLogger(Nya::INFO, "info.log"); // file

	l_trace << "First string";
	auto log = l_info << "Second string";
	log << " continues";
}
*/

#include "EnumNya.hpp"
#include "SingletonNya.hpp"
#include "NyaQt.hpp"

#include <QString>
#include <QVariant>
#include <QVector>


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
	LogLevel level;

public:
	Logger(LogLevel level);
	virtual ~Logger() = 0;

	LogLevel GetLogLevel() const { return level; }

	virtual void Write(const QString& message) = 0;
};

// ~~~
class Log : public QObject, public Singleton<Log>
{
	Q_OBJECT
	QString timeFormat = NYA_DT_FORMAT;       /// yyyy-MM-dd hh:mm:ss
	QString messageFormat = "[%1] %2 [%3:%4] "; /// time level file line
	QVector<s_p<Logger>> loggers;               /// loggers list

public:
	Log();
	QString GetTimeFormat() const { return timeFormat; }
	QString GetMessageFormat() const { return messageFormat; }
	void SetTimeFormat(const QString& timeFormat) { this->timeFormat = timeFormat; }
	void SetMessageFormat(const QString& messageFormat) { this->messageFormat = messageFormat; }

#ifdef Q_OS_WIN
	void AddConsoleLogger(LogLevel level, const QByteArray& codecName = "CP1251");
#else
	void AddConsoleLogger(LogLevel level, const QByteArray& codecName = "UTF-8");
#endif
	void AddFileLogger(LogLevel level, const QString& filePath, bool isRewrite = false
					  , const QByteArray& codecName = "UTF-8");
	void AddLogger(s_p<Logger> logger);

	void WriteAll(const QString& message, LogLevel level = TRACE);

private slots:
	void OnLog(const QString& message) { WriteAll(message); }
};

// ~~~
struct LogStreamShared;
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
