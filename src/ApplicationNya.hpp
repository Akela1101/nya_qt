/****************************************************
 *
 * Copyright (c) 2013 Akela1101 <akela1101@gmail.com>
 *
 ****************************************************/

#ifndef ApplicationNya_H
#define ApplicationNya_H

#include "ConfigNya.hpp"
#include "Nya.hpp"


namespace Nya
{
	class Application : public QObject
	{
		Q_OBJECT
		friend void SystemSignalHandler(int iSignal);

	protected:
		QString appName;              /// application name
		QString rootConfigDir;        /// system configuration directory
		QString configDir;            /// configuration directory
		QString configFileFullName;   /// full config name
		QString logDir;               /// logs directory
		QString traceLogPath;         /// logDir + trace.log
		QString crashLogPath;         /// logDir + crash.log
		QString robotEmail;           /// e-mail for alerts
		Config config;                /// config
		bool isDaemon;                /// daemon or usual
		bool isRestartOnCrash;        /// restart on crash

	public:
		Application() : isDaemon(false), isRestartOnCrash(false) {}
		virtual ~Application() = 0;

		QString GetConfigDir() const { return configDir; }
		const Config& GetConfig() const { return config; }

		virtual bool Init(); /// must be called!

	protected:
		bool SaveConfig();
		bool LoadConfig(QString configDir_ = "", QString configFileName = "");
		void InitLogs();

	protected slots:
		virtual void Quit();

	signals:
		void SignalConfigLoaded();
	};
}

#endif // ApplicationNya_H
