#include <iostream>

#include <basedefs.h>
#include <pluginmanager.h>
//#include <core_api_cpp.h>
//#include <core_frontend.h>
#include <networkmanager_frontend.h>
#include <networkmanager_api_cpp.h>
#include <networkcache_userinterface.h>
#include <channelcache.h>
#include <unistd.h> // for Sleep
#include <string.h>
#include <sstream>

static pthread_mutex_t g_stateMutex;
static anp::uint32 g_state = 0;
using namespace anp;
using namespace firc;

void irc_onPrivMsg(INetworkManagerFrontend &network,
					const anp::int8 *sender,
					const anp::int8 *receiver,
					const anp::int8 *message)
{
	std::cout << "main.cpp: Received a PRIVMSG!" << std::endl;

	if ( 0 == strcmp(message, "die") )
	{
		// Write state
		pthread_mutex_lock(&g_stateMutex);
		g_state = 1; // QUIT
		pthread_mutex_unlock(&g_stateMutex);
	}

	if ( receiver[0] == '#' )
	{
		const NetworkCacheUserInterface &cache = network.networkCache();
		const ChannelCache *channel = cache.getChannel(receiver);
	
		std::stringstream ss;
		ss << "PRIVMSG " << channel->name() << " :The topic for "
			<< channel->name() << " is " << channel->topic() << ".\r\n";
		network.sendMessage(ss.str());
	}
}

int main(int argc, char *argv[])
{
	using namespace anp;
	using namespace anp::firc;
	
	const int8 *pluginNames[] = {
	//	"./libpluginTest1.so"
	};
	
	pthread_mutex_init(&g_stateMutex, NULL);

	PluginManager pluginManager;
	
	for ( uint32 i=0; i<sizeof(pluginNames)/sizeof(pluginNames[0]);
			i++ )
	{
		pluginManager.loadPlugin(pluginNames[i]);
	}
	
	INetworkManagerFrontend *chatJunkies =
		networkmanager_create("irc.chatjunkies.org", "6667",
												&pluginManager);

	
	anp::uint32 state = 0;
	pluginManager.addCallbackOnPrivMsg(irc_onPrivMsg);

	sleep(9);
	chatJunkies->sendMessage("JOIN #my-secret-botdev\r\n");
	chatJunkies->sendMessage(
					"PRIVMSG #my-secret-botdev :Hello world!\r\n");
	sleep(20);
	
	// When the quit command has been received, state will equal 1.
	while ( 0 == state )
	{
		// Read state
		pthread_mutex_lock(&g_stateMutex);
		state = g_state;
		pthread_mutex_unlock(&g_stateMutex);
		sleep(1); // Just to conserve CPU..
	}
	
	// Quit
	//chatJunkies->setQuitMessage("Time to go! See you ChatJunkies!");
	networkmanager_destroy(chatJunkies);
	std::cout << "main.cpp: Successfully disconnected."	<< std::endl;
	
	pthread_mutex_destroy(&g_stateMutex);
	
	return 0;
}
