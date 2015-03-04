/****************************************************
 *
 * Copyright (c) 2013 Akela1101 <akela1101@gmail.com>
 *
 ****************************************************/

#include "DirNya.hpp"
#include "TimeNya.hpp"

#include <QCoreApplication>
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QTextCodec>
#include <QTranslator>
#include <cstdio>
#include <csignal>
#ifdef Q_OS_WIN
#    include <windows.h>
#endif

#include "ApplicationNya.hpp"


namespace Nya
{
#ifndef Q_OS_WIN
#include <cxxabi.h>
#include <execinfo.h>
/**
 * Demangle function names.
 *
 * e.g.: * _ZN4nyan4nyanE → nyan::nyan()
 */
static const char* Demangle(const char* symbol)
{
	char* temp = new char[256];
	// first, try to demangle a c++ name
	if( 1 == sscanf(symbol, "%*[^(]%*[^_]%255[^)+]", temp) )
	{
		size_t size;
		int status;
		if( char* demangled = abi::__cxa_demangle(temp, 0, &size, &status) )
		{
			return demangled;
		}
	}
	// if that didn't work, try to get a regular c symbol
	if( 1 == sscanf(symbol, "%255s", temp) )
	{
		return temp;
	}
	// if all else fails, just return the symbol
	return symbol;
}
#else
static const char* Demangle(const char* symbol) { return symbol; }
static size_t backtrace(void**, int) { return 0; }
static char** backtrace_symbols(void**, size_t) { return 0; }

/*
  If you want backtrace on windows, add dbghelp library.
	  dlltool -k -d libdbghelp.def -l dbghelp.a

#include <Dbghelp.h>
static size_t backtrace(void *stack[], int stackSize)
{
//	HANDLE pProcess = GetCurrentProcess();
//	SymInitialize(pProcess, 0, true);

	return CaptureStackBackTrace(0, stackSize, stack, 0);
}

static char** backtrace_symbols(void *stack[], size_t size)
{
	HANDLE pProcess = GetCurrentProcess();

	char** ret = new char*[size];
	for( int i = 0; i < size; ++i )
	{
		SYMBOL_INFO* symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256, 1);
		symbol->MaxNameLen   = 255;
		symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		SymFromAddr(pProcess, (DWORD)stack[ i ], 0, symbol);

		ret[i] = symbol->Name;
	}
	return ret;
}*/
#endif

/**
 * Function for catching system signals like SIGSEGV.
 */
static Application* pApp;
void SystemSignalHandler(int iSignal)
{
	if( iSignal == SIGTERM )
	{
		l_info << "User requested termination.";
		pApp->Quit();
		return;
	}

	// add crash log to log
	l_fail << "Catched segfault!";
	void *stack[512];
	size_t size = backtrace(stack, 512);

	char** s = backtrace_symbols(stack, size);
	for( uint i = 2; i < size; ++i )
	{
		l_fail << Demangle(s[i]);
	}

	// move log → log_crash
	QFile::rename(pApp->traceLogPath, pApp->crashLogPath);

	if( pApp->isRestartOnCrash )
	{
		QStringList args;
		if( !pApp->isDaemon ) args << "-n";
		l_fail << "Restart app: "<< QProcess::startDetached(QCoreApplication::applicationFilePath(),
														   args, QCoreApplication::applicationDirPath());
	}
	signal(iSignal, SIG_DFL);
	l_fail << "Quick exit from crashed app.";
#ifndef Q_OS_WIN
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
#else
	rootConfigDir = QDir::homePath() + "/.config/";
#endif

	// global variable!
	pApp = this;

	// sistem signal handler
	signal(SIGSEGV, SystemSignalHandler);
	signal(SIGTERM, SystemSignalHandler);
	return true;
}

/**
 * Correct exit.
 */
void Application::Quit()
{
	l_info << "Quit application called!";
	InvMet(qApp, "quit", Qt::QueuedConnection);
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

	// save updated config
	SaveConfig();

	// translations
	QTranslator* translator = new QTranslator(qApp);
	QString lang = config["LANG"];
	if( lang.size() && translator->load(lang + ".qm", ":/tr") )
	{
		qApp->installTranslator(translator);
	}

	// logs
	InitLogs();
	l_info << "Config in [" << configDir << "]";
	l_trace << "Logs (LOG_DIR) in [" << logDir << "]";

	// run once (mutex is also used in inno setup)
#ifdef Q_OS_WIN
	QByteArray appId = config["APP_ID"].toUtf8();
	if( !appId.size() ) appId = appName.toUtf8();
	CreateMutexA(0, 0, appId);
	if( ERROR_ALREADY_EXISTS == GetLastError() )
	{
		l_info << "Cannot start. Application \"" + appId + "\" is already running.";
		return false;
	}
#endif
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
	QString mainLogPath = logDir + "main.log";
	traceLogPath = logDir + "trace.log";
	crashLogPath = logDir + "crash.log";

	// add loggers
	Log::GS().AddLogger(TRACE);
	Log::GS().AddLogger(TRACE, traceLogPath, true);
	Log::GS().AddLogger(INFO, mainLogPath);

	// check for crashes
	QFileInfo crashLogInfo(crashLogPath);
	if( !crashLogInfo.exists() )
	{
		isRestartOnCrash = true;
	}
	else
	{
		QFile crashFile(crashLogPath);
		crashFile.open(QIODevice::ReadOnly);
		char sTime[22];
		crashFile.read(sTime, 21);
		QDateTime crashLogInfoCreatedTime = QDateTime::fromString(QString(sTime), NYA_TIME_FORMAT);

		// check if too frequent restarts
		int delta = timeUTC() - crashLogInfoCreatedTime.toTime_t();
		if( delta > 300 ) // 5 min.
		{
			l_info << "Crash delta = " << delta << " time = " << sTime;
			isRestartOnCrash = true;

			QFile::rename(crashLogPath, logDir + "crashToSend.log");
			//todo: virtual function for network send mail.
		}
	}
}
}
