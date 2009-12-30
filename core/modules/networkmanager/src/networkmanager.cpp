#include "../networkmanager.h"
#include <tokenizer.h>
#include <tcpconnection.h>
#include <pluginmanager.h>
#include <plugin_jobs.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <memory>

namespace anp
{
namespace firc
{
	static void *threadRunMessageReceiver(void *arg)
	{
		NetworkManager *nm = (NetworkManager *)arg;
		if ( NULL != nm )
		{
			try
			{
				nm->runMessageReceiver();
			} catch ( std::exception &e )
			{
				std::cout
					<< "NetworkManager-MessageReceiverRunner: Exception occured: "
					<< e.what() << std::endl;
			}
		}
		std::cout << "NetworkManager: closing thread." << std::endl;
		pthread_exit(0);
	}
	
	NetworkManager::NetworkManager(const int8 *host, const int8 *port,
									PluginManager *pluginManager):
	m_state(CONNECTING),
	m_connection(host, port),
	m_messageSender(new MessageSender(m_connection)),
	m_pluginManager(pluginManager)
	{
		Result res = RES_FAILED;
		std::string m_outStr;
		
		std::cout << "Successfully connected." << std::endl;
		
		m_state = REGISTERING;
		// Nick
		m_outStr = std::string("NICK ")
			+std::string("fircbot09")
			+std::string("\r\n");
		m_connection.send(m_outStr);
		// User
		m_outStr = std::string("USER ")
			+std::string("fircbot09")
			+std::string(" 0 * :")
			+std::string("Anton Petersson")
			+std::string("\r\n");
		m_connection.send(m_outStr);
		
		//m_networkCache.init(host, "fircbot09");
		
		
		m_state = CONNECTED;
			
		m_receiverThread.create(NULL,	threadRunMessageReceiver,
								(void *)this);
	}
	
	NetworkManager::~NetworkManager()
	{

	}
	
	void NetworkManager::deinit(const int8 *message)
	{
		m_stateMutex.lock();
		m_state = SHUTTING_DOWN;
		m_stateMutex.unlock();
		
		std::string quitMessage("QUIT :");
		quitMessage += message;
		quitMessage += "\r\n";
		m_connection.send(quitMessage);
		
		std::cout << "NetworkManager: pthread_join..." << std::endl;
		m_receiverThread.join(NULL);
		std::cout << "NetworkManager: receiver thread dead..." << std::endl;
	}
	
	Result NetworkManager::runMessageReceiver()
	{
		using tokenizer::tokenize;
		State state = UNKNOWN;
		bool32 connected = true;

		char buffer[MAX_DATA_LENGTH];
		bool hasPrefix=false;
		std::string in,
					currentMessage,
					prefix,
					command,
					leftOvers,
					target,
					sender,
					temp1,
					temp2,
					temp3;
					
		std::cout << "runMessageReceiver():start" << std::endl;
		
		while ( TRUE == connected )
		{
			//sleep(2);
			//std::cout << "messageReceiver: hi" << std::endl;
			
			m_stateMutex.lock();
			state = m_state;
			m_stateMutex.unlock();
			
			if ( state == SHUTTING_DOWN )
			{
				break;
			}
			
			
			/////////////////////////////////////////

			// 1. Receive a message
			if ( RES_CONNECTION_CLOSED == m_connection.receive(buffer,
				 sizeof(buffer)/sizeof(buffer[0])) )
			{
				// The server closed the connection
				// Flag we're not connected anymore, and exit thread
				connected = FALSE;
				return RES_CONNECTION_CLOSED;
			}
			in = buffer; // optimize..?
			std::cout << "recv:    " << in << std::endl;

			// 2. Tokenize it and parse it
			// Start by dividing it up into several messages
			// (sometimes the server
			// will send several messages at the same time)
			while ( 1 ) {
				if ( tokenize(currentMessage, in, "\r\n") ) {
					// if there was something left from the previous
					// parsing run, add it here
					if ( leftOvers.length() > 0 ) {
						currentMessage = leftOvers + currentMessage;
						leftOvers.erase();
					}

					// Reset old vars as needed
					prefix.erase();
					// Start parsing the current message
					if ( currentMessage[0] == ':' ) {
						// The message has a prefix
						currentMessage.erase(0, 1);
						// Get the prefix
						tokenize(prefix, currentMessage, ' ');
					}
					// Get the command
					tokenize(command, currentMessage, ' ');

					if ( command == "PING" ) {
						 // Strip the starting colon ':'
						currentMessage.erase(0, 1);
						temp3 = "PONG :";
						temp3 += currentMessage;
						temp3 += "\r\n";
						std::cout << "receiver: Sending mgs=" << temp3
							<< std::endl;
						m_connection.send(temp3);
						//#m_pluginManager->onPing(currentMessage);
					} else if ( command == "PRIVMSG" ) {
						tokenize(target, currentMessage, ' ');

						currentMessage.erase(0, 1);
						std::cout << "(local) PRIVMSG detected: "
							<< currentMessage << std::endl;

						PrivMsgJob job(	NULL,
										(void *)this,
										prefix.c_str(),
										target.c_str(),
										currentMessage.c_str());
						m_pluginManager->performJob(&job,
											PluginManager::IRC_PRIVMSG);
					} else if ( command == "JOIN" ) {
						std::cout << "(local) JOIN command detected: "
							<< currentMessage << std::endl;
						if ( currentMessage[0] == ':' )
						{
 							// Remove ':' in front of the channel
 							// name (efnet seems to use it)
							currentMessage.erase(0, 1);
						}

						// Separate the prefix into nickname,
						// user and host
						tokenize(temp1, prefix, '!'); // nickname
						tokenize(temp2, prefix, '@'); // user
						// prefix now only holds the host

						//m_networkCache.onJoin(temp1,
						//					  temp2,
						//					  prefix,
						//					  currentMessage);

						m_networkCache.addUserToChannel(temp1, temp2,
												prefix, currentMessage);
						
						
						JoinJob joinJob(NULL,
										(void *)this,
										currentMessage.c_str(),
										temp1.c_str());
						m_pluginManager->performJob(&joinJob,
											PluginManager::IRC_JOIN);
					} else if ( command == "PART" ) {
						if ( currentMessage[0] == ':' )
						 	// Remove ':' in front of the channel name
							currentMessage.erase(0, 1);

						// Separate the prefix into nickname, user
						// and host (UNNECESSARY FOR PART MESSAGE !?
						//  ONLY USES NICKNAME ANYWAY!!!)
						tokenize(temp1, prefix, '!'); // nickname
						tokenize(temp2, prefix, '@'); // user
						// prefix now only holds the host

						//m_networkCache.onPart(temp1,
						//					  temp2,
						//					  prefix,
						//					  currentMessage);

						m_networkCache.removeUserFromChannel(temp1,
														currentMessage);

						//#m_pluginManager->onPart(temp1,
						// currentMessage);
					} else if ( command == "TOPIC" ) {
						// topic change.. ex: (GTANet)
						// [Original message]
						// ":Pliskin!IceChat7@gtanet-ADE03D4F.bsnet.se
						// TOPIC #antons_kanal :Det här är en topic"
						tokenize(target, currentMessage, " :");

						m_networkCache.setTopic(target, currentMessage);
						//m_pluginManager->onTopicChange(); TODO

						// ##### NUMERIC REPLIES BELOW ######
					} else if ( command == "332" ) {
						// RPL_TOPIC ex: (GTANet)
						//[Original message]
						// ":frosties.de.eu.gtanet.com 332
						// firc_04 #antons_kanal :lalala"
						
						 // target = firc_04
						tokenize(target, currentMessage, ' ');
						 // temp1 = #antons_kanal
						tokenize(temp1, currentMessage, " :");
						
						m_networkCache.setTopic(temp1, currentMessage);
					} else if ( command == "333" ) {
						// "RPL_TOPICWHOWHEN" ? Ex: (ChatJunkies)
						// [original message] :at.chatjunkies.org 333
						// fircbot09 #my-secret-botdev
						// antonp!~antonp@ChatJunkies-6a09bfa2.dyn.
						// perspektivbredband.net 1248007592

					} else if ( command == "353" ) {
						// RPL_NAMREPLY ex: (efnet) [Original message]
						// :efnet.demon.co.uk 353 firc_04 =
						// #antons_kanal :firc_04 @Pliskin_
						
						 // target = firc_04
						tokenize(target, currentMessage, ' ');
						 // temp1 = '='
						tokenize(temp1, currentMessage, ' ');
						 // temp2 = #antons_kanal
						tokenize(temp2, currentMessage, " :");

						bool keepGoing = true;
						while ( keepGoing ) {
							// temp3 = [@|+|nothing]nickname
							keepGoing = tokenize(temp3,
												 currentMessage,
												 " ");
							switch ( temp3[0] ) {
							case '@':
								temp3.erase(0, 1);
								break;
							case '+':
								temp3.erase(0, 1);
								break;
							default:
								break;
							}
							//m_networkCache.addUser(temp2, temp3);
							m_networkCache.addUserToChannel(temp3, "", "",
																 temp2);
						}
					}
				} else {
					// The message isn't complete, so save it and
					// add the rest when it arrives [todo]
					//m_out << "[Parsing results] The message wasn't
					// complete. (Looked like: " << currentMessage
					// << ")\n";
					if ( currentMessage.length() >= 3 )
					{
						leftOvers = currentMessage.erase(
							currentMessage.length()-3);
					}
					break;
				}
				if ( in.length() == 0 )
					break;
			}			
		}
		return RES_OK;
	}
	
	void NetworkManager::sendMessage(const std::string &message)
	{
		/** @todo
		Create a message sender which can run in it's own thread
		and work as an anti-flood mechanism.
		*/
		
		m_connection.send(message);
	}

	const NetworkCacheUserInterface &NetworkManager::networkCache() const
	{
		return m_networkCache;
	}

} // namespace firc
} // namespace anp
