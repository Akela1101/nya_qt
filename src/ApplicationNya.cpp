/****************************************************
 *
 * Author: Akela1101 <akela1101@gmail.com>
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
#include "QxtBasicFileLoggerEngine"
#ifdef Q_OS_WIN
#include <windows.h>
#endif
#include <cstdio>
#include <csignal>

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
	// First, try to demangle a c++ name.
	if( 1 == sscanf(symbol, "%*[^(]%*[^_]%255[^)+]", temp) )
	{
		size_t size;
		int status;
		if( char* demangled = abi::__cxa_demangle(temp, 0, &size, &status) )
		{
			return demangled;
		}
	}
	// If that didn't work, try to get a regular c symbol.
	if( 1 == sscanf(symbol, "%255s", temp) )
	{
		return temp;
	}
	// If all else fails, just return the symbol.
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

	// Add crash log to log.
	l_critical << "Catched segfault!";
	void *stack[512];
	size_t size = backtrace(stack, 512);

	char** s = backtrace_symbols(stack, size);
	for( uint i = 2; i < size; ++i )
	{
		l_critical << Demangle(s[i]);
	}

	// Move log → log_crash
	QFile::rename(pApp->traceLogPath, pApp->crashLogPath);

	if( pApp->isRestartOnCrash )
	{
		QStringList args;
		if( !pApp->isDaemon ) args << "-n";
		l_critical << "Restart app: "<< QProcess::startDetached(QCoreApplication::applicationFilePath(),
														   args, QCoreApplication::applicationDirPath());
	}
	signal(iSignal, SIG_DFL);
	l_critical << "Quick exit from crashed app.";
#ifndef Q_OS_WIN
	quick_exit(3);
#endif
}

//=============================================================
Application::~Application()
{}

/**
 * Инициализация.
 */
bool Application::Init()
{
	appName = QFileInfo(QCoreApplication::applicationFilePath()).baseName();
	if( !appName.size() ) appName = "_unknown_application_name_";

	// Сделать мьютекс, чтобы в inno setup выключать.
#ifdef Q_OS_WIN
	CreateMutexA(0, 0, appName.toUtf8()); /*
	if( ERROR_ALREADY_EXISTS == GetLastError() )
	{
		l_info << "Cannot start. Application already running.";
		exit(2);
	}*/
#endif

	// Задать кодек по умолчанию для QString из сишных строк.
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	QTextCodec::setCodecForTr(codec);
	QTextCodec::setCodecForCStrings(codec);
	QTextCodec::setCodecForLocale(codec);

	// Выставлять текущую директорию там, где бинарник.
	QDir::setCurrent(QCoreApplication::applicationDirPath());

	// Задать главную папку с системными конфигами всех программ.
#ifdef Q_OS_WIN
	rootConfigDir = MakeDirPath(QDir::fromNativeSeparators(qgetenv("APPDATA")));
#else
	rootConfigDir = QDir::homePath() + "/.config/";
#endif

	// Глобальная переменная на случай сбоя.
	pApp = this;

	// Задать обработчик системных прерываний.
	signal(SIGSEGV, SystemSignalHandler);
	signal(SIGTERM, SystemSignalHandler);
	return true;
}

/**
 * Корректный выход.
 */
void Application::Quit()
{
	l_info << "Quit application called!";
	InvMet(qApp, "quit", Qt::QueuedConnection);
}

/**
 * Сохранить настройки.
 */
bool Application::SaveConfig()
{
	// Сохранить обновлённый конфиг.
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
 * Загрузить настройки.
 */
void Application::LoadConfig(QString configDir_, QString configFileName)
{
	// Задать имя конфига.
	if( !configFileName.size() ) configFileName = appName + ".cfg";

	// Если папка явно указана, тогда выбирать её.
	if( configDir_.size() )
	{
		configDir = MakeDirPath(configDir_);
		if( !MakeDirIfNone(configDir) )
		{
			configDir = "";
		}
	}
	// Иначе попытаться создать конфиги в домашней или в текущей папке.
	if( !configDir.size() )
	{
		// Создать директорию с конфигами.
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

	// Загрузить конфиг по умолчанию.
	QString defaultConfigFileName = ":/" + configFileName;
	QFile defaultConfigFile(defaultConfigFileName);
	if( defaultConfigFile.open(QIODevice::ReadOnly | QIODevice::Text) )
	{
		QTextStream itsDefault(&defaultConfigFile);
		config.Load(itsDefault);
		defaultConfigFile.close();
	}

	// Обновить существующим конфигом, если есть.
	QFile configFile(configFileFullName);
	if( configFile.open(QIODevice::ReadOnly | QIODevice::Text) )
	{
		QTextStream its(&configFile);
		config.Load(its, false);
		configFile.close();
	}

	// Обновить константным конфигом, если есть.
	QFile constConfigFile(defaultConfigFileName + ".const");
	if( constConfigFile.open(QIODevice::ReadOnly | QIODevice::Text) )
	{
		QTextStream its(&constConfigFile);
		config.Load(its, false);
		constConfigFile.close();
	}

	// Сохранить обновлённый конфиг.
	SaveConfig();

	// Переводы.
	QTranslator* translator = new QTranslator(qApp);
	QString lang = config["LANG"];
	if( lang.size() && translator->load(lang + ".qm", ":/tr") )
	{
		qApp->installTranslator(translator);
	}

	// Инициализировать логи.
	InitLogs();
	l_info << "Config in [" << configDir << "]";
	l_trace << "Logs (LOG_DIR) in [" << logDir << "]";
}

/**
 * Инициализировать логи.
 * Кроме того, если последний сбой был давно, разрешить перезапуск.
 *
 * true, если лог сбоя есть и начат давно → отослать его админу.
 */
void Application::InitLogs()
{
	// Если папка для логов не указана, назначить для этого папку с конфигами.
	logDir = config["LOG_DIR"];
	logDir = logDir.size() ? Nya::MakeDirPath(logDir) : configDir;

	QString mainLogPath = logDir + "main.log";
	traceLogPath = logDir + "trace.log";
	crashLogPath = logDir + "crash.log";

	Nya::MakeDirIfNone(logDir);
	QFile::remove(traceLogPath);

	QxtBasicFileLoggerEngine *traceLogFile  = new QxtBasicFileLoggerEngine(traceLogPath);
	qxtLog->addLoggerEngine("trace", traceLogFile);
	qxtLog->setMinimumLevel("trace", QxtLogger::TraceLevel);
	QxtBasicFileLoggerEngine *mainLogFile  = new QxtBasicFileLoggerEngine(mainLogPath);
	qxtLog->addLoggerEngine("main", mainLogFile);
	qxtLog->setMinimumLevel("main", QxtLogger::InfoLevel);

	QFileInfo crashLogInfo(crashLogPath);
	// Если предыдущего лога нет, или есть, но начат давно, разрешить перезапуск в-случае-чего.
	if( !crashLogInfo.exists() )
	{
		isRestartOnCrash = true;
	}
	else
	{
		QFile crashFile(crashLogPath);
		crashFile.open(QIODevice::ReadOnly);
		char s[22]; // Чтение времени.
		crashFile.read(s, 21);
		QDateTime crashLogInfoCreatedTime = QDateTime::fromString(QString(s), TIME_FORMAT);

		// Проверка на случай, если перезапускается очень часто.
		int delta = timeUTC() - crashLogInfoCreatedTime.toTime_t();
		if( delta > 300 ) // 5 min.
		{
			l_info << "Crash delta = " << delta << " time = " << QDateTime::fromTime_t(timeUTC());
			isRestartOnCrash = true;

			QFile::rename(crashLogPath, logDir + "crashToSend.log");
			//todo: virtual function for network send mail.
		}
	}
}
}
