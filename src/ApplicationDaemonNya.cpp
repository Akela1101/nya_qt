#include <QtGlobal>

#ifdef Q_OS_UNIX
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#endif

#include "ApplicationDaemonNya.hpp"

using namespace std;


namespace nya
{
/**
 * Init.
 */
bool ApplicationDaemon::Init(int argc, char* argv[])
{
	if (!Application::Init()) return false;
	
	// Демонизация:
#ifndef Q_OS_UNIX
	(void) argc;
	(void) argv;
#else
	if( argc == 1 || (string(argv[1]) != "--no-daemon" && string(argv[1]) != "-n") )
	{
		isDaemon = true;

		// Создаем дочерний процесс.
		pid_t pid = fork();
		if( pid < 0 )
		{
			exit(1);
		}
		else if( pid )
		{
			exit(0);
		}

		// Даём права на работу с фс.
		umask(0);

		// Генерируем уникальный индекс процесса.
		pid_t sid = setsid();
		if( sid < 0 )
		{
			exit(1);
		}

		// Убрать вывод в консоль.
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
	}
#endif
	return true;
}
}
