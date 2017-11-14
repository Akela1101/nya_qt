#ifndef ApplicationNya_H
#define ApplicationNya_H

#include <QObject>

#include "ConfigNya.hpp"
#include "NyaQt.hpp"


namespace nya
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
	~Application() override = 0;
	
	QString GetConfigDir() const { return configDir; }
	const Config& GetConfig() const { return config; }
	
	virtual bool Init(); /// must be called!

protected:
	bool SaveConfig();
	bool LoadConfig(QString configDir_ = "", QString configFileName = "");

private:
	void InitLogs();
	void SendCrash();

protected slots:
	virtual void Quit();

signals:
	void SignalConfigLoaded();
	void SignalMessageCrash(QString text);
	void SignalCrashSent(); // call on sent
};
}

#endif // ApplicationNya_H
