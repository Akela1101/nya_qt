#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QTextCodec>

#include "LogNya.hpp"


namespace Nya
{
//=================================================================
// General
Logger::Logger(LogLevel level)
	: level(level)
	, codec(QTextCodec::codecForName("UTF-8"))
{}

/**
 * Output codec: UTF-8, UTF-16, CP1251, KOI-8R, ...
 */
void Logger::SetOutputCodec(const QByteArray& outputCodec)
{
	codec = QTextCodec::codecForName(outputCodec);
}

// Console
class LoggerConsole : public Logger
{
	QFile f;

public:
	LoggerConsole(LogLevel level)
		: Logger(level)
	{
		f.open(level <= INFO ? stdout : stderr, QIODevice::WriteOnly);
	}

	virtual void Write(const QString& message)
	{
		f.write(codec->fromUnicode(message));
		f.flush();
	}
};

// File
class LoggerFile : public Logger
{
	QFile f;

public:
	LoggerFile(LogLevel level, const QString& filePath, bool isRewrite)
		: Logger(level)
		, f(filePath)
	{
		f.open(QIODevice::WriteOnly | (isRewrite ? QIODevice::Truncate : QIODevice::Append));
	}
	~LoggerFile() { f.close(); }

	virtual void Write(const QString& message)
	{
		f.write(codec->fromUnicode(message));
		f.flush();
	}
};

//=================================================================
/**
 * Add console output.
 */
Logger& Log::AddLogger(LogLevel level)
{
	auto logger = make_s_p<LoggerConsole>(level);
	loggers.push_back(logger);
	return *logger;
}

/**
 * Add output to file.
 */
Logger& Log::AddLogger(LogLevel level, const QString& filePath, bool isRewrite)
{
	auto logger = make_s_p<LoggerFile>(level, filePath, isRewrite);
	loggers.push_back(logger);
	return *logger;
}

/**
 * Write message to all loggers.
 */
void Log::WriteAll(const QString& message, LogLevel level)
{
	for( auto& logger : loggers )
	{
		if( logger->level <= level ) logger->Write(message);
	}
}

//=================================================================
struct LogStreamShared
{
	LogLevel level;
	QString message;

	LogStreamShared(LogLevel level, const char* file, int line)
		: level(level)
	{
		const QString& timeFormat = Log::GS().GetTimeFormat();
		message += QString(Log::GS().GetMessageFormat())
				.arg(QDateTime::currentDateTime().toString(timeFormat))
				.arg(etos(level))
				.arg(QFileInfo(file).fileName())
				.arg(line);
	}
	~LogStreamShared()
	{
		Log::GS().WriteAll(message + '\n', level);
	}
};

//=================================================================
LogStream::LogStream(LogLevel level, const char* file, int line)
	: shared(new LogStreamShared(level, file, line))
{}

/**
 * Cache data.
 */
LogStream& LogStream::operator<< (const QVariant& value)
{
	shared->message += value.toString();
	return *this;
}
}
