#include <iostream>

#include <basedefs.h>
#include <fircapi.h>
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

	Result res = RES_FAILED;
	void *fircCore = NULL;
	void *chatJunkies = NULL;
	
	const int8 *pluginNames[] = {
		"./libpluginTest1.so"
	};
	
	pthread_mutex_init(&g_stateMutex, NULL);

	res = coreCreate(&fircCore, sizeof(pluginNames)/sizeof(int8 *),
						pluginNames);
	std::cout << "fircCore = " << fircCore << std::endl;
	if ( RES_OK != res )
	{
		std::cout << "Failed to create the firc core object! Halting!"
			<< std::endl;
		return 1;
	}
	
	std::cout << "Successfully created the firc core object!"
		<< std::endl;
	
	res = ircConnect(fircCore, "irc.chatjunkies.org", "6667",
						&chatJunkies);
	
	if ( RES_OK == res )
	{
		anp::uint32 state = 0;
		res = ircAddOnPrivMsgCallback(fircCore, irc_onPrivMsg);
		if ( RES_OK != res )
		{
			std::cout << "Failed to add privmsg callback." << std::endl;
		}
		sleep(5);
		ircSendRaw(chatJunkies, "JOIN #my-secret-botdev\r\n");
		ircPrivMsg(chatJunkies, "#my-secret-botdev", "Hello world!");			
		
		sleep(20);
		
		// When the quit command has been received, state will equal 1.
		while ( 0 == state )
		{
			// Read state
			pthread_mutex_lock(&g_stateMutex);
			state = g_state;
			pthread_mutex_unlock(&g_stateMutex);
			sleep(3); // Just to conserve CPU..
		}
		
		// Quit
		res = ircDisconnect(fircCore, chatJunkies,
							"Time to go! See you ChatJunkies!");
		if ( RES_OK == res )
		{
			std::cout << "main.cpp: Successfully disconnected."
				<< std::endl;
		} else
		{
			std::cout << "main.cpp: Failed to disconnect"
				<< std::endl;	
		}
	} else
	{
		std::cout << "main.cpp: Failed to connect to ChatJunkies:"
			<< res << std::endl;
	}
	
	coreDestroy(fircCore);
	pthread_mutex_destroy(&g_stateMutex);
	
	return 0;
}
