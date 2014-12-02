#ifndef ApplicationNya_H
#define ApplicationNya_H

#include "SingletonNya.hpp"
#include "ConfigNya.hpp"

#include "Nya.hpp"


namespace Nya
{
	class ApplicationBase : public QObject
	{
		Q_OBJECT

		friend void SystemSignalHandler(int iSignal);

	protected:
		QString appName;              /// имя приложения
		QString rootConfigDir;        /// путь к корню конфигов
		QString configDir;            /// путь к конфигу
		QString configFileFullName;   /// имя конфига вместе с путём
		QString logDir;               /// загружается из конфига в переменной LOG_DIR
		QString traceLogPath;         /// logDir + trace.log
		QString crashLogPath;         /// logDir + crash.log
		QString robotEmail;           /// e-mail для оповещений.
		Config config;                /// конфиг
		bool isDaemon;                /// демон/обычное приложение
		bool isRestartOnCrash;        /// перезапускаться ли при падании

	public:
		QString GetConfigDir() const { return configDir; }

		virtual void Quit();

	protected:
		ApplicationBase();
		virtual ~ApplicationBase() = 0;

		bool LoadConfig(QString configDir_ = "", QString configFileName = "");
		bool SaveConfig();
		bool InitLogs();

	private slots:
		void OnQuit() { Quit(); }
	};
}

#endif // ApplicationNya_H
