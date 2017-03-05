/****************************************************
 *
 * Copyright (c) 2015 Akela1101 <akela1101@gmail.com>
 *
 ****************************************************/

#include <QDateTime>
#include <QtGlobal>
#include <QFile>
#include <QFileInfo>
#include <QTextCodec>
#include <QTextStream>
#include <QMutex>

#include "LogNya.hpp"


namespace Nya
{
//=================================================================
// General
Logger::Logger(LogLevel level)
	: level(level)
{}

Logger::~Logger() {}

// Console
class LoggerConsole : public Logger
{
	QFile f;
	QTextCodec* codec;

public:
	LoggerConsole(LogLevel level, const QByteArray& codecName)
		: Logger(level)
		, codec(QTextCodec::codecForName(codecName))
	{
		f.open(stdout, QIODevice::WriteOnly);
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
	QTextCodec* codec;

public:
	LoggerFile(LogLevel level, const QString& filePath, bool isRewrite, const QByteArray& codecName)
		: Logger(level)
		, f(filePath)
		, codec(QTextCodec::codecForName(codecName))
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
 * QDebug output.
 */
#if QT_VERSION < 0x050000
void WriteQDebug(QtMsgType type, const char* message)
{
	switch( type )
	{
	case QtDebugMsg:    NyaLog.WriteAll(message, TRACE); return;
	case QtWarningMsg:  NyaLog.WriteAll(message, INFO); return;
	case QtCriticalMsg: NyaLog.WriteAll(message, ERROR); return;
	case QtFatalMsg:    NyaLog.WriteAll(message, FAIL); return;
	}
}
#else
void WriteQDebug(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
	switch( type )
	{
	case QtDebugMsg:    LogStream(TRACE, context.file, context.line) << message; return;
	case QtWarningMsg:  LogStream(INFO, context.file, context.line) << message; return;
	case QtCriticalMsg: LogStream(ERROR, context.file, context.line) << message; return;
	case QtFatalMsg:    LogStream(FAIL, context.file, context.line) << message; return;
	}
}
#endif

//=================================================================
Log::Log()
{
#if QT_VERSION < 0x050000
	qInstallMsgHandler(WriteQDebug);
#else
	qInstallMessageHandler(WriteQDebug);
#endif
}

/**
 * Add console output.
 * Output codecs: UTF-8, UTF-16, CP1251, IBM866, KOI-8R, ...
 */
void Log::AddConsoleLogger(LogLevel level, const QByteArray& codecName)
{
	auto logger = make_s<LoggerConsole>(level, codecName);
	loggers.push_back(logger);
}

/**
 * Add output to file.
 */
void Log::AddFileLogger(LogLevel level, const QString& filePath, bool isRewrite
        , const QByteArray& codecName)
{
	auto logger = make_s<LoggerFile>(level, filePath, isRewrite, codecName);
	loggers.push_back(logger);
}

/**
 * Add custom logger.
 */
void Log::AddLogger(s_p<Logger> logger)
{
    loggers.push_back(logger);
}

/**
 * Write message to all loggers.
 */
void Log::WriteAll(const QString& message, LogLevel level)
{
	static QMutex mutex;
	mutex.lock();
	for( auto& logger : loggers )
	{
		if( logger->GetLogLevel() <= level ) logger->Write(message);
	}
	mutex.unlock();
}

//=================================================================
struct LogStreamShared
{
	LogLevel level;
	QString message;

	LogStreamShared(LogLevel level, const char* file, int line)
		: level(level)
	{
		if( file )
		{
			message += QString(NyaLog.GetMessageFormat())
				.arg(QDateTime::currentDateTime().toString(NyaLog.GetTimeFormat()))
				.arg(etos(level))
				.arg(QFileInfo(file).fileName())
				.arg(line);
		}
	}
	~LogStreamShared()
	{
		NyaLog.WriteAll(message + '\n', level);
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
