#include "../networkmanager.h"
#include <tokenizer.h>
#include <tcpconnection.h>
#include <pluginmanager.h>

#include <unistd.h>
#include <iostream>
#include <sstream>

namespace firc
{
	static void *threadRunMessageReceiver(void *arg)
	{
		NetworkManager *nm = (NetworkManager *)arg;
		if ( NULL != nm )
		{
			nm->runMessageReceiver();
		}
		pthread_exit(0);
	}
	
	NetworkManager::NetworkManager()
	{
//		m_stateMutex = PTHREAD_MUTEX_INITIALIZER;
	}
	
	Result NetworkManager::init(PluginManager *pluginManager)
	{
		Result res = RES_FAILED;
		std::string m_outStr;
		
		m_pluginManager = pluginManager;
		
		m_state = CONNECTING;
		res = m_connection.connect("irc.efnet.ch", "6667");
		if ( res != RES_OK )
		{
			return res;
		}
		std::cout << "Successfully connected." << std::endl;
		
		m_state = REGISTERING;
		// Nick
		m_outStr = std::string("NICK ")+std::string("fircbot09")+std::string("\r\n");
		m_connection.send(m_outStr);
		// User
		m_outStr = std::string("USER ")+std::string("fircbot09")+std::string(" 0 * :")+std::string("Anton Petersson")+std::string("\r\n");
		m_connection.send(m_outStr);
		// Handle the first ping command
		m_connection.send("PING\r\n");
		m_connection.send("PONG\r\n");
		
		m_connection.send("JOIN #my-secret-botdev\r\n");
		
		
		m_state = CONNECTED;
		pthread_create(&m_receiverThread, NULL,
						threadRunMessageReceiver, (void *)this);
		
		
		
		
		return RES_OK;
	}
	
	void NetworkManager::deinit()
	{
		pthread_mutex_lock(&m_stateMutex);
		m_state = SHUTTING_DOWN;
		pthread_mutex_unlock(&m_stateMutex);
		
		pthread_join(m_receiverThread, NULL);
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
			sleep(2);
			std::cout << "messageReceiver: hi" << std::endl;
			
			pthread_mutex_lock(&m_stateMutex);
			state = m_state;
			pthread_mutex_unlock(&m_stateMutex);
			
			if ( state == SHUTTING_DOWN )
			{
				break;
			}
			
			
			/////////////////////////////////////////

			// 1. Receive a message
			if ( RES_CONNECTION_CLOSED == m_connection.receive(buffer, sizeof(buffer)/sizeof(buffer[0])) ) {
				// The server closed the connection
				// Flag we're not connected anymore, and exit thread
				connected = FALSE;
				//m_eventProcessor.addEvent(m_agentID, -1, -1, "-- Disconnected from server, by server --\n");
				return RES_CONNECTION_CLOSED;
			}
			in = buffer; // optimize..?
			std::cout << "recv:    " << in << std::endl;
			//m_out << "[Before parsing, the message looks like] " << in << '\n';

			// 2. Tokenize it and parse it
			// Start by dividing it up into several messages (sometimes the server
			// will send several messages at the same time)
			while ( 1 ) {
				if ( tokenize(currentMessage, in, "\r\n") ) {
					// if there was something left from the previous parsing run, add it here
					if ( leftOvers.length() > 0 ) {
						currentMessage = leftOvers + currentMessage;
						leftOvers.erase();
					}
					//m_out << "<Original message> " << currentMessage << '\n';
					std::stringstream s2;
					s2 << "[Original message] " << currentMessage << '\n';
					//m_eventProcessor.addEvent(m_agentID, -1, -1, s2.str());

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
						currentMessage.erase(0, 1); // Strip the starting colon ':'
						//#m_pluginManager->onPing(currentMessage);
					} else if ( command == "PRIVMSG" ) {
						tokenize(target, currentMessage, ' ');
	//					m_pluginManager->onPrivMsg(

						//std::stringstream s;
						//s << "PRIVMSG DETECTED: [Prefix] '" << prefix << "' [target] '" << target << "' [msg] '" << currentMessage.erase(0, 1) << "'\n";
						//m_eventProcessor.addEvent(m_agentID, -1, -1, s.str());
						//m_eventProcessor.addEvent(m_agentID, -1, -1, s.str());
						//m_eventProcessor.addEvent(m_agentID, -1, -1, s.str());
						currentMessage.erase(0, 1);

						//#m_pluginManager->onPrivMsg(prefix, target, currentMessage);
					} else if ( command == "JOIN" ) {
						std::cout << "(local) JOIN command detected: " << currentMessage << std::endl;
						if ( currentMessage[0] == ':' )
							currentMessage.erase(0, 1); // Remove ':' in front of the channel name (efnet seems to use it)

						// Separate the prefix into nickname, user and host
						tokenize(temp1, prefix, '!'); // nickname
						tokenize(temp2, prefix, '@'); // user
						// prefix now only holds the host

						//#m_cache->onJoin(temp1, temp2, prefix, currentMessage);
						m_pluginManager->irc_onJoin((void *)this, currentMessage.c_str(), temp1.c_str());
					} else if ( command == "PART" ) {
						if ( currentMessage[0] == ':' )
							currentMessage.erase(0, 1); // Remove ':' in front of the channel name

						// Separate the prefix into nickname, user and host (UNNECESSARY FOR PART MESSAGE !? ONLY USES NICKNAME ANYWAY!!!)
						tokenize(temp1, prefix, '!'); // nickname
						tokenize(temp2, prefix, '@'); // user
						// prefix now only holds the host

						//#m_cache->onPart(temp1, temp2, prefix, currentMessage);
						//#m_pluginManager->onPart(temp1, currentMessage);
					} else if ( command == "TOPIC" ) {
						// topic change.. ex: (GTANet) [Original message] :Pliskin!IceChat7@gtanet-ADE03D4F.bsnet.se TOPIC #antons_kanal :Det här är en topic
						tokenize(target, currentMessage, " :");

						//#m_cache->setTopic(target, currentMessage);
						//m_pluginManager->onTopicChange(); TODO

						// ##### NUMERIC REPLIES BELOW ######
					} else if ( command == "332" ) {
						// RPL_TOPIC ex: (GTANet) [Original message] :frosties.de.eu.gtanet.com 332 firc_04 #antons_kanal :lalala
						tokenize(target, currentMessage, ' '); // target = firc_04
						tokenize(temp1, currentMessage, " :"); // temp1 = #antons_kanal
						
						//#m_cache->setTopic(temp1, currentMessage);
					} else if ( command == "333" ) {
						// "RPL_TOPICWHOWHEN" ? Ex ?
					} else if ( command == "353" ) {
						// RPL_NAMREPLY ex: (efnet) [Original message] :efnet.demon.co.uk 353 firc_04 = #antons_kanal :firc_04 @Pliskin_
						tokenize(target, currentMessage, ' '); // target = firc_04
						tokenize(temp1, currentMessage, ' '); // temp1 = '='
						
						tokenize(temp2, currentMessage, " :"); // temp2 = #antons_kanal

						bool keepGoing = true;
						while ( keepGoing ) { // temp3 = [@|+|nothing]nickname
							keepGoing = tokenize(temp3, currentMessage, " ");
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
							//#m_cache->addUser(temp2, temp3);
						}
					}

					//m_out << "[Parsing results] Prefix: " << prefix << " Command: " << command << " The rest: " << currentMessage << '\n';
				} else {
					// The message isn't complete, so save it and add the rest when it arrives [todo]
					//m_out << "[Parsing results] The message wasn't complete. (Looked like: " << currentMessage << ")\n";
					if ( currentMessage.length() >= 3 ) // Test added august 25th
						leftOvers = currentMessage.erase(currentMessage.length()-3);
					break;
				}
				if ( in.length() == 0 )
					break;
			}

			// 3. Update registry (if needed)

			// 4. Assign tasks to addons (or perhaps to the ActionTaker directly in some cases)
			/////////////////////////////////////////
			
			
		}
		return RES_OK;
	}
	
	Result NetworkManager::sendMessage(const std::string &message)
	{
		/** @todo
		Create a message sender which can run in it's own thread
		and work as an anti-flood mechanism.
		*/
		
		m_connection.send(message);
		
		return RES_OK;
	}
	
	
	/////////////////////////////
	// Native functions below
	/////////////////////////////
	
	extern "C"
	{
		/**
		 * @brief
		 * Sends a PRIVMSG to the irc server.
		 * 
		 * @param target
		 * Target can be either the name of a channel or a user.
		 * 
		 * @param message
		 * The message to send.
		 */
		void ircPrivMsg(void *network, const int8 *target,
						const int8 *message)
		{
			NetworkManager *nm = (NetworkManager *)network;
			if ( NULL == nm )
			{
				return;
			}
			
			std::string ircMessage("PRIVMSG ");
			ircMessage += target;
			ircMessage += " :";
			ircMessage += message;
			ircMessage += "\r\n";
			
			nm->sendMessage(ircMessage);
		}
		
		/**
		 * @brief
		 * Sends a raw string to the irc server.
		 * 
		 * @param network
		 * IRC network to send to.
		 * 
		 * @param rawCommand
		 * The string to send. Note that CRLF ("\r\n") isn't added, so you
		 * might need to add that to the end of the string.
		 */
		void ircSendRaw(void *network, const int8 *rawCommand)
		{
			NetworkManager *nm = (NetworkManager *)network;
			if ( NULL == nm )
			{
				return;
			}
			
			nm->sendMessage(std::string(rawCommand));
		}
	}
}
