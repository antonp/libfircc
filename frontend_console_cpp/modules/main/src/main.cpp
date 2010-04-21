#include <iostream>

#include <basedefs.h>
#include <pluginmanager.h>
//#include <core_api_cpp.h>
//#include <core_frontend.h>
#include <networkmanager_frontend.h>
#include <networkmanager_api_cpp.h>
#include <networkcache_userinterface.h>
#include <messageprefix.h>
#include <channelcache.h>
#include <unistd.h> // for Sleep
#include <string.h>
#include <sstream>
#include <log.h>
#include <fstream>
#include <tcpconnection.h>
#include <networkmanagerevents.h>
#include <eventdispatcher.h>
#include <networkmanagerevents.h>
#include <appevents.h>
#include <appeventdispatchers.h>

static pthread_mutex_t g_stateMutex;
static anp::uint32 g_state = 0;
using namespace anp;
using namespace firc;

class LogFileWriter: public ILogInterface
{
public:
	LogFileWriter(const std::string &filename):
	m_filename(filename)
	{
	}
	void present(const anp::dstring &message)
	{
		std::ofstream fout(m_filename.c_str(),
							std::ios_base::app | std::ios_base::out);
		if ( fout.is_open() )
		{
			fout << message << std::endl;
			fout.close();
		}
	}
private:
	std::string m_filename;
};

void irctest_onJoin(events::Join &event)
{
	std::cout << "irctest_onJoin: " << event.origin().nick()
		<< " joined " << event.channel() << std::endl;
}

class EventHandler: public events::ISubscriber<events::Join>,
					public events::ISubscriber<events::Part>,
					public events::ISubscriber<events::PrivMsg>,
					public events::ISubscriber<events::Topic>,
					public events::ISubscriber<events::NumericReply>
{
public:
	void receiveEvent(events::Join &event)
	{
		irctest_onJoin(event);
	}
	void receiveEvent(events::Part &event)
	{
		std::cout << "[main.cpp <-] " << event.origin().nick()
			<< " left channel " << event.channel() << ". ("
			<< event.message() << ")" << std::endl;
	}
	void receiveEvent(events::PrivMsg &event)
	{
		std::cout << "[main.cpp <-] " << event.origin().nick() << ": "
			<< event.message() << std::endl;


		if ( event.message() == "die" )
		{
			// Write state
			pthread_mutex_lock(&g_stateMutex);
			g_state = 1; // QUIT
			pthread_mutex_unlock(&g_stateMutex);
		}

		if ( event.target()[0] == '#' && event.message() == "topic?" )
		{
			const NetworkCacheUserInterface &cache =
				event.network().networkCache();
			ChannelCache channel;
			cache.getChannel(event.target(), channel);
	
			std::stringstream ss;
			ss << "PRIVMSG " << channel.name() << " :The topic for "
				<< channel.name() << " is " << channel.topic() << ".\r\n";
			event.network().sendMessage(ss.str());
		}
	}
	void receiveEvent(events::Topic &event)
	{
		std::cout << "[main.cpp <-] " << event.origin().nick()
			<< " changed the topic for " << event.channel() << " to '"
			<< event.topic() << "'" << std::endl;
	}
	void receiveEvent(events::NumericReply &event)
	{
		std::cout << "[main.cpp <-] Numeric reply "
			<< event.command() << " received. ("
			<< event.origin().prefix() << ')' << std::endl;
	}
};

bool32 waitForSocket(int socket,
					uint32 timeoutSeconds,
					uint32 timeoutMicroseconds)
{
	timeval timeout;
	timeout.tv_sec = timeoutSeconds;
	timeout.tv_usec = timeoutMicroseconds;
	fd_set readFileDescriptorSet;
	FD_ZERO(&readFileDescriptorSet);
	FD_SET(socket, &readFileDescriptorSet);
	if ( (-1) == ::select(socket+1, &readFileDescriptorSet,
					NULL, NULL, &timeout) )
	{
		throw NetworkException("select() returned -1");
	}
	return FD_ISSET(socket, &readFileDescriptorSet);
}

int main(int argc, char *argv[])
{
	using namespace anp;
	using namespace anp::firc;

	app::EventDispatchers dispatchers;
	
	std::string serverAddress = "irc.chatjunkies.org",
				serverPort = "6667";

	if ( argc >= 3 )
	{
		serverAddress = argv[1];
		serverPort = argv[2];
	}

	const int8 *pluginNames[] = {
	//	"./libpluginTest1.so"
	};

	LogFileWriter logFileWriter("frontend_cpp.log");
	Log log;
	log.addLogInterface(&logFileWriter);
	log.addMessage("Log initialized.");
	
	pthread_mutex_init(&g_stateMutex, NULL);

	PluginManager pluginManager;
	
	for ( uint32 i=0; i<sizeof(pluginNames)/sizeof(pluginNames[0]);
			i++ )
	{
		pluginManager.loadPlugin(pluginNames[i]);
	}
	
	INetworkManagerFrontend *network =
		networkmanager_create(serverAddress.c_str(), serverPort.c_str(),
													&pluginManager);
	
	anp::uint32 state = 0;

	EventHandler eventHandler;
	network->eventDispatcherJoin().subscribe(&eventHandler);
	network->eventDispatcherPart().subscribe(&eventHandler);
	network->eventDispatcherPrivMsg().subscribe(&eventHandler);
	network->eventDispatcherTopic().subscribe(&eventHandler);
	network->eventDispatcherNumericReply().subscribe(&eventHandler);

	network->runMessageReceiverInThread();

	sleep(9);
	network->sendMessage("JOIN #my-secret-botdev\r\n");
	network->sendMessage(
					"PRIVMSG #my-secret-botdev :Hello world!\r\n");
	sleep(20);
	
	// When the quit command has been received, state will equal 1.
	while ( 0 == state )
	{
		if ( waitForSocket(fileno(stdin), 0, 250000) )
		{
			std::string command;
			std::getline(std::cin, command);
			if ( command == "die" )
			{
				break;
			} else
			{
				network->sendMessage(command+"\r\n");
			}
		}

		// Read state
		pthread_mutex_lock(&g_stateMutex);
		state = g_state;
		pthread_mutex_unlock(&g_stateMutex);
		sleep(1); // Just to conserve CPU..
	}
	
	// Quit
	network->deinit("Time to go! See you!");
	networkmanager_destroy(network);
	log.addMessage("main.cpp: Successfully disconnected.");
	
	pthread_mutex_destroy(&g_stateMutex);
	
	return 0;
}
