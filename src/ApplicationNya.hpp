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

	public:
		Application() : isDaemon(false) {}
		virtual ~Application() = 0;

		QString GetConfigDir() const { return configDir; }
		const Config& GetConfig() const { return config; }

		virtual bool Init(); /// must be called!

	protected:
		/// on success crash.log will be removed
		virtual bool OnCrashLog(const QString& crashLogPath) { (void)crashLogPath; return false; }

		bool SaveConfig();
		bool LoadConfig(QString configDir_ = "", QString configFileName = "");

	private:
		void InitLogs();
		void SendCrash();

	protected slots:
		virtual void Quit();

	signals:
		void SignalConfigLoaded();
	};
}

#endif // ApplicationNya_H
