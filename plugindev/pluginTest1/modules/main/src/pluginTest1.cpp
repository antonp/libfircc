#include <basedefs.h>
#include <fircapi.h>
#include <iostream>
#include <sstream>
#include <string.h>

using namespace firc;

extern "C"
{

void *g_fircCore = NULL;

void irc_onJoin(void *network, const int8 *channel, const int8 *user)
{
	std::cout << "pluginTest1.cpp: onJoin: channel='" << channel
		<< "' user:'" << user << "'" << std::endl;
		
	ircPrivMsg(network, channel,
						"Hello world from pluginTest1.cpp");
}

void irc_onPrivMsg(void *network, const int8 *sender,
					const int8 *receiver, const int8 *message)
{
	Result res = RES_FAILED;
	void *pm = NULL;
	uint32 pluginCount = 0;
	std::stringstream ss;

	res = coreGetPluginManager(g_fircCore, &pm);
	if ( RES_OK != res )
	{
		std::cout << "Failed to get plugin manager.." << std::endl;
	} else if ( message[0] == '?' )
	{
		pluginGetPluginCount(pm, &pluginCount);
		ss << pluginCount << " plugins loaded." << std::endl;
		ircPrivMsg(network, receiver, ss.str().c_str());
		ss.str("");
		
		for ( uint32 i=0; i<pluginCount; ++i )
		{
			int8 name[256];

			res = pluginGetPluginInfo(pm, i,
										name, 256 );
			if ( RES_OK == res )
			{
				ss << name << ", ";
			}
		}
		ircPrivMsg(network, receiver, ss.str().c_str());
	} else if ( message[0] == ']' )
	{
		res = pluginLoad(pm, "./libpluginTest2.so");
		if ( RES_OK == res )
		{
			std::cout << "Successfully loaded pluginTest2."
				<< std::endl;
		} else
		{
			std::cout << "Failed to load pluginTest2." << std::endl;
		}
	} else if ( message[0] == '%' )
	{
		pluginGetPluginCount(pm, &pluginCount);
		
		for ( uint32 i=0; i<pluginCount; ++i )
		{
			int8 name[256];

			res = pluginGetPluginInfo(pm, i,
										name, 256 );
			if ( RES_OK == res )
			{
				if ( 0 == strcmp(name, "./libpluginTest2.so") )
				{
					pluginUnload(pm, i, 1);
				}
			}
		}
	}
}

uint32 pluginInit(void *fircCore)
{
	Result res = RES_FAILED;
	void *network = NULL;
	
	g_fircCore = fircCore;

	std::cout << "Hello world from pluginTest1.cpp!" << std::endl;

/*	res = ircConnect(fircCore, "irc.efnet.ch", "6667", &network);
	if ( RES_OK == res )
	{
		std::cout << "pluginTest1.cpp: Successfully connected!"
			<< std::endl;
			
		ircSendRaw(network, "JOIN #my-secret-botdev\r\n");
		ircPrivMsg(network, "#my-secret-botdev", "Hello world!");			
		
		// Quit
		res = ircDisconnect(fircCore, network, "Time to go! See you!");
		if ( RES_OK == res )
		{
			std::cout << "pluginTest1.cpp: Successfully disconnected." << std::endl;
		} else
		{
			std::cout << "pluginTest1.cpp: Failed to disconnect" << std::endl;	
		}

	} else
	{
		std::cout << "pluginTest1.cpp: Failed to connect" << std::endl;
	}
*/
	return 1;
}

void pluginDeinit()
{
	std::cout << "Good bye world from pluginTest1.cpp" << std::endl;
}

} // extern "C"
