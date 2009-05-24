#include <basedefs.h>
#include <fircapi.h>
#include <iostream>

using namespace firc;

extern "C"
{

uint32 pluginInit()
{
	std::cout << "Hello world from pluginTest1.cpp!" << std::endl;

	firc::ircPrivMsg(NULL, "#my-secret-botdev", "Hello world!");

	return 0;
}

void pluginDeinit()
{
	std::cout << "Good bye world from pluginTest1.cpp" << std::endl;
}

} // extern "C"
