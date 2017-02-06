/****************************************************
 *
 * Copyright (c) 2013 Akela1101 <akela1101@gmail.com>
 *
 ****************************************************/

#include "DirNya.hpp"
#include "TimeNya.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QTextCodec>
#include <QTextStream>
#include <QTimer>
#include <QTranslator>
#include <cstdio>
#include <csignal>
#ifdef __GNUC__
#    include <cxxabi.h>
#endif
#ifdef Q_OS_WIN
#    include <windows.h>
#    include <Dbghelp.h>
#endif

#include "ApplicationNya.hpp"


namespace Nya
{
static const int stackSize = 100;
static const int symbolLength = 256;

#ifndef Q_OS_WIN
#    include <execinfo.h>
#endif
/**
 * Backtrace functions for windows (doesn't work properly sometimes >.<).
 *
 * For static backtrace on windows:
 *   dlltool -k -d libdbghelp.def -l dbghelp.a
 */
static size_t backtrace(void *stack[], int stackSize)
{
	HANDLE pProcess = GetCurrentProcess();
	//SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_UNDNAME | SYMOPT_LOAD_LINES);
	SymInitialize(pProcess, 0, true);
	return CaptureStackBackTrace(0, stackSize, stack, 0);
}
static char** backtrace_symbols(void *stack[], size_t size)
{
	HANDLE pProcess = GetCurrentProcess();
	char** ret = new char*[size];
	for( int i = 0; i < size; ++i )
	{
		SYMBOL_INFO* symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + symbolLength, 1);
		symbol->MaxNameLen = symbolLength - 1;
		symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		SymFromAddr(pProcess, (intptr_t)stack[i], 0, symbol);
		ret[i] = symbol->Name;
	}
	return ret;
}

/**
 * Demangle function names.
 *
 * e.g.: _ZN4nyan4nyanE → nyan::nyan()
 */
#ifdef __GNUC__
static const char* Demangle(const char* symbol)
{
	// add _ before Z for mingw
	if( symbol[0] == 'Z' )
	{
		char* symbol2 = new char[symbolLength + 1];
		symbol2[0] = '_';
		strcpy(symbol2 + 1, symbol);
		symbol = symbol2;
	}
	//char* sDemangled = new char[symbolLength];
	//sscanf(symbol, "%*[^(]%*[^_]%255[^)+]", sDemangled);
	int status;
	if( char* sDemangled = abi::__cxa_demangle(symbol, 0, 0, &status) )	return sDemangled;
	return symbol;
}
#elif defined _MSC_VER
static const char* Demangle(const char* symbol)
{
	char* sDemangled = new char[symbolLength];
	if( UnDecorateSymbolName(symbol, sDemangled, symbolLength, 0) ) return sDemangled;
	return symbol;
}
#else
static const char* Demangle(const char* symbol) { return symbol; }
#endif

/**
 * Function for catching system signals like SIGSEGV.
 */
static Application* pApp;
static QDateTime tAppStart;
void SystemSignalHandler(int iSignal)
{
	if( iSignal == SIGTERM )
	{
		l_info << "User requested termination.";
		pApp->Quit();
		return;
	}

	// add backtrace to log
	l_fail << "Application crashed:";
	void *stack[stackSize];
	size_t size = backtrace(stack, stackSize);
	char** s = backtrace_symbols(stack, size);
	for( uint i = 0; i < size; ++i )
	{
		if( *s[i] ) o_fail << Demangle(s[i]);
	}

	// if running long enough (5 min), restart.
	if( tAppStart.secsTo(QDateTime::currentDateTime()) > 300 )
	{
		pApp->SendCrash();

		// restart app
		QStringList args = qApp->arguments();
		args.pop_front();
		if( !pApp->isDaemon ) args << "-n" << "-r"; //todo: -d == not -n
		l_fail << "Restart app: "
			   << QProcess::startDetached(QCoreApplication::applicationFilePath(),
										  args, QCoreApplication::applicationDirPath());
	}
	signal(iSignal, SIG_DFL);

#ifndef Q_OS_WIN
	l_fail << "Quick exit from crashed app.";
	quick_exit(3);
#endif
}

//=============================================================
Application::~Application()
{}

/**
 * Init.
 */
bool Application::Init()
{
	// global variables!
	pApp = this;
	tAppStart = QDateTime::currentDateTime();

	// app name
	appName = QFileInfo(QCoreApplication::applicationFilePath()).baseName();
	if( !appName.size() ) appName = "_unknown_application_name_";

	// default QString convertion encoding
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	QTextCodec::setCodecForLocale(codec);
#if QT_VERSION < 0x050000
	QTextCodec::setCodecForCStrings(codec);
#endif

	// set current directory in ./
	QDir::setCurrent(QCoreApplication::applicationDirPath());

	// set system config folder
#ifdef Q_OS_WIN
	rootConfigDir = MakeDirPath(QDir::fromNativeSeparators(qgetenv("APPDATA")));
	SetErrorMode(0x7); // disable useless dialog boxes for errors
#else
	rootConfigDir = QDir::homePath() + "/.config/";
#endif

	// sistem signal handler
	signal(SIGABRT, SystemSignalHandler);
	signal(SIGILL, SystemSignalHandler);
	signal(SIGFPE, SystemSignalHandler);
	signal(SIGSEGV, SystemSignalHandler);
	signal(SIGTERM, SystemSignalHandler);
	return true;
}

/**
 * Save configuration.
 */
bool Application::SaveConfig()
{
	// save updated config
	QFile configFile(configFileFullName);
	if( !configFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text) )
	{
		l_error << "Can't write: " << configFileFullName;
		return false;
	}
	QTextStream ots(&configFile);
	config.Save(ots);
	configFile.close();
	configFile.setPermissions((QFile::Permissions)0x664);
	return true;
}

/**
 * Load configuration.
 */
bool Application::LoadConfig(QString configDir_, QString configFileName)
{
	// set config name
	if( !configFileName.size() ) configFileName = appName + ".cfg";

	// set folder from argument if there is
	if( configDir_.size() )
	{
		configDir = MakeDirPath(configDir_);
		if( !MakeDirIfNone(configDir) )
		{
			configDir = "";
		}
	}
	// else try home or current folder
	if( !configDir.size() )
	{
		configDir = rootConfigDir + appName + "/";
		if( !MakeDirIfNone(configDir) )
		{
			configDir = QDir::currentPath() + "/.config/";
			if( !MakeDirIfNone(configDir) )
			{
				configDir = "";
			}
		}
	}
	configFileFullName = configDir + configFileName;

	// load default config
	QString defaultConfigFileName = ":/" + configFileName;
	QFile defaultConfigFile(defaultConfigFileName);
	if( defaultConfigFile.open(QIODevice::ReadOnly | QIODevice::Text) )
	{
		QTextStream itsDefault(&defaultConfigFile);
		config.Load(itsDefault);
		defaultConfigFile.close();
	}

	// update with current config
	QFile configFile(configFileFullName);
	if( configFile.open(QIODevice::ReadOnly | QIODevice::Text) )
	{
		QTextStream its(&configFile);
		config.Load(its, false);
		configFile.close();
	}

	// update with constant config
	QFile constConfigFile(defaultConfigFileName + ".const");
	if( constConfigFile.open(QIODevice::ReadOnly | QIODevice::Text) )
	{
		QTextStream its(&constConfigFile);
		config.Load(its, false);
		constConfigFile.close();
	}

	// logs
	InitLogs();

	// run once (mutex is also used in inno setup)
#ifdef Q_OS_WIN
	QByteArray appId = config["APP_ID"].toUtf8();
	if( !appId.size() ) appId = appName.toUtf8();
	CreateMutexA(0, 0, appId);
	if( ERROR_ALREADY_EXISTS == GetLastError() && !qApp->arguments().contains("-r") )
	{
		l_info << "Cannot start. Application \"" + appId + "\" is already running.";
		Quit();
		return false;
	}
#endif

	// save updated config
	SaveConfig();

	// translations
	QTranslator* translator = new QTranslator(qApp);
	QString lang = config["LANG"];
	if( lang.size() && translator->load(lang + ".qm", ":/tr") )
	{
		qApp->installTranslator(translator);
	}

	l_trace << "Config in [" << configDir << "]";
	l_trace << "Logs (LOG_DIR) in [" << logDir << "]";

	emit SignalConfigLoaded();
	return true;
}

/**
 * Initialize logs.
 * Save crash log if any.
 */
void Application::InitLogs()
{
	// set config dir as logs dir
	logDir = config["LOG_DIR"];
	logDir = logDir.size() ? Nya::MakeDirPath(logDir) : configDir;
	Nya::MakeDirIfNone(logDir);

	// set file names
	QString infoLogPath = logDir + "info.log";
	traceLogPath = logDir + "trace.log";
	crashLogPath = logDir + "crash.log";

	// add loggers
	NyaLog.AddLogger(TRACE);
	NyaLog.AddLogger(TRACE, traceLogPath, true);
	NyaLog.AddLogger(INFO, infoLogPath);
}

/**
 * Send crash log.
 */
void Application::SendCrash()
{
	// copy trace.log → crash.log (move can fail)
	QFile::remove(crashLogPath);
	QFile::copy(traceLogPath, crashLogPath);

	QFile fCrash(crashLogPath);
	if( fCrash.open(QIODevice::ReadOnly) && receivers(SIGNAL(SignalMessageCrash(QString))) )
	{
		emit SignalMessageCrash(fCrash.readAll());

		// wait for crash be sent
		QEventLoop loop;
		connect(this, SIGNAL(SignalCrashSent()), &loop, SLOT(quit()));
		QTimer::singleShot(5000, &loop, SLOT(quit()));
		loop.exec();
	}
}

/**
 * Correct exit.
 */
void Application::Quit()
{
	l_info << "Quit application called!";
	InvMet(qApp, "quit", Qt::QueuedConnection);
}
}
