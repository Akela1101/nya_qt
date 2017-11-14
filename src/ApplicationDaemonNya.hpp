#ifndef APPLICATIONDAEMONNYA_H
#define APPLICATIONDAEMONNYA_H

#include "ApplicationNya.hpp"


namespace nya
{
class ApplicationDaemon : public Application
{
public:
	bool Init(int argc, char* argv[]);
};
}

#endif // APPLICATIONDAEMONNYA_H
