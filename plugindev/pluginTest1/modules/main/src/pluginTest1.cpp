#include <basedefs.h>
#include <fircapi.h>
#include <iostream>

using namespace firc;

extern "C"
{

void irc_onJoin(void *network, const int8 *channel, const int8 *user)
{
	std::cout << "pluginTest1.cpp: onJoin: channel='" << channel
		<< "' user:'" << user << "'" << std::endl;
		
	ircPrivMsg(network, channel,
						"Hello world from pluginTest1.cpp");
}

uint32 pluginInit()
{
	std::cout << "Hello world from pluginTest1.cpp!" << std::endl;

	ircPrivMsg(NULL, "#my-secret-botdev", "Hello world!");

	return 0;
}

void pluginDeinit()
{
	std::cout << "Good bye world from pluginTest1.cpp" << std::endl;
}

} // extern "C"
