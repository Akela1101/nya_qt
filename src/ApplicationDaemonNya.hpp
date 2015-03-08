#ifndef APPLICATIONDAEMONNYA_H
#define APPLICATIONDAEMONNYA_H

#include "ApplicationNya.hpp"


namespace Nya
{
	class ApplicationDaemonBase : public Application
	{
	public:
		bool Init(int argc, char *argv[]);
	};
}

#endif // APPLICATIONDAEMONNYA_H
