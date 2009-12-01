#include <iostream>

#include <basedefs.h>
#include <core_api_cpp.h>
#include <core_frontend.h>
#include <networkmanager_frontend.h>
#include <unistd.h> // for Sleep
#include <string.h>

static pthread_mutex_t g_stateMutex;
static anp::uint32 g_state = 0;

void irc_onPrivMsg(void *network,
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
}

int main(int argc, char *argv[])
{
	using namespace anp;
	using namespace anp::firc;
	
	const int8 *pluginNames[] = {
		"./libpluginTest1.so"
	};
	
	pthread_mutex_init(&g_stateMutex, NULL);

	ICoreFrontend *core = core_create(
									sizeof(pluginNames)/sizeof(int8 *),
									pluginNames);
	
	std::cout << "Successfully created the firc core object!"
		<< std::endl;
	
	INetworkManagerFrontend *chatJunkies =
		core->createNetworkManager("irc.chatjunkies.de", "6667");
	
	anp::uint32 state = 0;
	core->addCallbackOnPrivMsg(irc_onPrivMsg);

	sleep(7);
	chatJunkies->sendMessage("JOIN #my-secret-botdev\r\n");
	chatJunkies->sendMessage("PRIVMSG #my-secret-botdev :Hello world!");	
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
	core->destroyNetworkManager(chatJunkies,
							    "Time to go! See you ChatJunkies!");
	std::cout << "main.cpp: Successfully disconnected."	<< std::endl;
	
	core_destroy(core);
	pthread_mutex_destroy(&g_stateMutex);
	
	return 0;
}
