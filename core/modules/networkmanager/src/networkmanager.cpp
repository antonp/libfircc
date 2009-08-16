#include "../networkmanager.h"
#include <tokenizer.h>
#include <tcpconnection.h>
#include <pluginmanager.h>

#include <iostream>
#include <sstream>

namespace anp
{
namespace firc
{
	static void *threadRunMessageReceiver(void *arg)
	{
		NetworkManager *nm = (NetworkManager *)arg;
		if ( NULL != nm )
		{
			nm->runMessageReceiver();
		}
		std::cout << "NetworkManager: closing thread." << std::endl;
		pthread_exit(0);
	}
	
	NetworkManager::NetworkManager()
	{
		threading::createMutexObject(&m_stateMutex);
	}
	
	NetworkManager::~NetworkManager()
	{
		threading::destroyMutexObject(m_stateMutex);
	}
	
	Result NetworkManager::init(const int8 *host, const int8 *port,
								PluginManager *pluginManager)
	{
		Result res = RES_FAILED;
		std::string m_outStr;
		
		m_pluginManager = pluginManager;
		
		m_state = CONNECTING;
		res = m_connection.connect(host, port);
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
		
		//m_connection.send("JOIN #my-secret-botdev\r\n");
		
		m_networkCache.init(host, "fircbot09");
		
		
		m_state = CONNECTED;
			
		res = m_receiverThread.create(NULL,	threadRunMessageReceiver,
			(void *)this);

		

		return res;
	}
	
	void NetworkManager::deinit(const int8 *message)
	{
		threading::mutexLock(m_stateMutex);
		m_state = SHUTTING_DOWN;
		threading::mutexUnlock(m_stateMutex);
		
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
			
			threading::mutexLock(m_stateMutex);
			state = m_state;
			threading::mutexUnlock(m_stateMutex);
			
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
						std::cout << "(local) PRIVMSG detected: " << currentMessage << std::endl;
						

						m_pluginManager->irc_onPrivMsg((void *)this,
												prefix.c_str(),
												target.c_str(),
												currentMessage.c_str());
					} else if ( command == "JOIN" ) {
						std::cout << "(local) JOIN command detected: " << currentMessage << std::endl;
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

						m_networkCache.onJoin(temp1, temp2, prefix, currentMessage);
						m_pluginManager->irc_onJoin((void *)this, currentMessage.c_str(), temp1.c_str());
					} else if ( command == "PART" ) {
						if ( currentMessage[0] == ':' )
							currentMessage.erase(0, 1); // Remove ':' in front of the channel name

						// Separate the prefix into nickname, user and host (UNNECESSARY FOR PART MESSAGE !? ONLY USES NICKNAME ANYWAY!!!)
						tokenize(temp1, prefix, '!'); // nickname
						tokenize(temp2, prefix, '@'); // user
						// prefix now only holds the host

						m_networkCache.onPart(temp1, temp2, prefix, currentMessage);
						//#m_pluginManager->onPart(temp1, currentMessage);
					} else if ( command == "TOPIC" ) {
						// topic change.. ex: (GTANet) [Original message] :Pliskin!IceChat7@gtanet-ADE03D4F.bsnet.se TOPIC #antons_kanal :Det här är en topic
						tokenize(target, currentMessage, " :");

						m_networkCache.setTopic(target, currentMessage);
						//m_pluginManager->onTopicChange(); TODO

						// ##### NUMERIC REPLIES BELOW ######
					} else if ( command == "332" ) {
						// RPL_TOPIC ex: (GTANet) [Original message] :frosties.de.eu.gtanet.com 332 firc_04 #antons_kanal :lalala
						tokenize(target, currentMessage, ' '); // target = firc_04
						tokenize(temp1, currentMessage, " :"); // temp1 = #antons_kanal
						
						m_networkCache.setTopic(temp1, currentMessage);
					} else if ( command == "333" ) {
						// "RPL_TOPICWHOWHEN" ? Ex: (ChatJunkies) [original message] :at.chatjunkies.org 333 fircbot09 #my-secret-botdev antonp!~antonp@ChatJunkies-6a09bfa2.dyn.perspektivbredband.net 1248007592

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
							m_networkCache.addUser(temp2, temp3);
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
	
	Result NetworkManager::getBotNickName(int8 *destName,
								uint32 maxNameLength) const
	{
		Result res = RES_INVALID_PARAMETER;
		
		if ( NULL != destName
			 && maxNameLength > 0 )
		{
			std::string tempName;
			uint32 i = 0;
			m_networkCache.getBotNickName(&tempName);
			
			for ( i=0; i<maxNameLength && tempName.size(); ++i )
			{
				destName[i] = tempName[i];
			}
			destName[i] = 0;
			res = RES_OK;
		}
		
		return res;
	}
	
	Result NetworkManager::getTopic(const int8 *channel,
									int8 *destTopic,
									uint32 maxTopicSize) const
	{
		Result res = RES_INVALID_PARAMETER;
		int32 ret = 0;
		
		if ( NULL != channel && NULL != destTopic && maxTopicSize > 0 )
		{
			std::string topic;
			uint32 i = 0;
			ret = m_networkCache.getTopic(&topic, channel);
			if ( -1 != ret )
			{
				for ( i=0; i<maxTopicSize-1 && i<topic.size(); ++i )
				{
					destTopic[i] = topic[i];
				}
				destTopic[i] = 0;
				res = RES_OK;
			} else
			{
				res = RES_NOTFOUND;
			}
		}
		
		return res;
	}
	
	Result NetworkManager::getChannelUserCount(const int8 *channel,
							uint32 *destChannelUserCount) const
	{
		Result res = RES_INVALID_PARAMETER;
		
		if ( NULL != channel
			 && NULL != destChannelUserCount )
		{
			if ( -1 != m_networkCache.getChannelUserCount(
												destChannelUserCount,
												channel) )
			{
				res = RES_OK;
			} else
			{
				res = RES_NOTFOUND;
			}
		}
	
		return res;	
	}
	
	Result NetworkManager::isUserInChannel(const int8 *channel,
						const int8 *nickName,
						bool32 *userIsInChannel) const
	{
		Result res = RES_INVALID_PARAMETER;
		
		if ( NULL != channel
			&& NULL != nickName
			&& NULL != userIsInChannel )
		{
			switch ( m_networkCache.isUserInChannel(channel, nickName) )
			{
			case -1:
				res = RES_NOTFOUND;
				break;
			case 0:
				res = RES_OK;
				*userIsInChannel = FALSE;
				break;
			case 1:
				res = RES_OK;
				*userIsInChannel = TRUE;
				break;
			default:
				res = RES_INTERNALERROR;
				break;
			}
		}
		
		return res;
	}
	
	Result NetworkManager::getChannelInfo(void *network,
							const int8 *channel,
							const void **channelInfo) const
	{
		Result res = RES_INVALID_PARAMETER;
		
		if ( NULL != network
			&& NULL != channel
			&& NULL != channelInfo )
		{
			// Make the call
			res = m_networkCache.createChannelInfoFromChannelName(
				channel,
				channelInfo);
		}
		
		return res;
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

		/**
		 * @brief
		 * Retrieves the nick of the bot/client.
		 * 
		 * @param network
		 * IRC network to look at.
		 * 
		 * @param destName
		 * String buffer that the nickname will be written to.
		 * 
		 * @param maxNameLength
		 * Size of the destination buffer in bytes.
		 * 
		 * @return
		 * RES_OK on success or an error code on failure.
		 */		
		Result cacheGetBotNickName(void *network, int8 *destName,
							uint32 maxNameLength)
		{
			Result res = RES_INVALID_PARAMETER;
			
			if ( NULL != network
				 && NULL != destName
				 && maxNameLength > 0 )
			{
				res = ((NetworkManager *)network)->getBotNickName(
														destName,
														maxNameLength);
			}
			
			return res;
		}
		
		/**
		 * @brief
		 * Retrieves the topic of a channel.
		 * 
		 * @param network
		 * IRC network to look at.
		 * 
		 * @param channel
		 * The channel from which the topic will be retrieved.
		 * 
		 * @param destTopic
		 * String buffer that the topic will be written to.
		 * 
		 * @param maxTopicSize
		 * Size of the destination buffer in bytes.
		 * 
		 * @return
		 * RES_OK on success or an error code on failure.
		 */
		Result cacheGetTopic(void *network, const int8 *channel,
					int8 *destTopic, uint32 maxTopicSize)
		{
			Result res = RES_INVALID_PARAMETER;
			
			if (   NULL != network
				&& NULL != channel
				&& NULL != destTopic
				&& 0 < maxTopicSize )
			{
				res = ((NetworkManager *)network)->getTopic(channel,
													destTopic,
													maxTopicSize);
			}
			
			return res;
		}

		/**
		 * @brief
		 * Retrieve the user count for a channel.
		 * 
		 * @param network
		 * IRC network to look at
		 * 
		 * @param channel
		 * The channel to retrieve the user count from.
		 * 
		 * @param destUserCount
		 * Pointer to a uint32 that will receive the user count.
		 * 
		 * @return
		 * RES_OK on success or an error code on failure.
		 */		
		Result cacheGetChannelUserCount(void *network,
										const int8 *channel,
										uint32 *destUserCount)
		{
			Result res = RES_INVALID_PARAMETER;
			
			if ( NULL != network
				 && NULL != channel
				 && NULL != destUserCount )
			{
				res = ((NetworkManager *)network)->getChannelUserCount(
														channel,
														destUserCount);
			}
			
			return res;
		}
		
		/**
		 * @brief
		 * Check if a user is in a channel.
		 * 
		 * @param network
		 * IRC network to look at.
		 * 
		 * @param channel
		 * Channel to look in.
		 * 
		 * @param userNick
		 * Nickname of the user to look for.
		 * 
		 * @param userIsInChannel
		 * Set to TRUE if the user is in the specified channel
		 * (according to the cache), FALSE otherwise.
		 * 
		 * @return
		 * RES_OK on success, RES_NOTFOUND if the channel can't be
		 * found or another error code if something else goes wrong.
		 */
		Result cacheIsUserInChannel(void *network, const int8 *channel,
								const int8 *userNick,
								bool32 *userIsInChannel)
		{
			Result res = RES_INVALID_PARAMETER;
			
			if ( NULL != network
				&& NULL != channel
				&& NULL != userNick
				&& NULL != userIsInChannel )
			{
				res = ((NetworkManager *)network)->isUserInChannel(
													channel,
													userNick,
													userIsInChannel);
			}
			
			return res;
		}
		
		/**
		 * @brief
		 * Retrieves various information about a specific channel that
		 * the bot is in.
		 * 
		 * @param[in] network
		 * The IRC network of the channel.
		 * 
		 * @param[in] channel
		 * The name of the channel (including '#' character).
		 * 
		 * @param[out] channelInfo
		 * Will point to a dynamically allocated copy of the internal
		 * channel info object stored in the cache. Remember to destroy
		 * it when you don't need it anymore.
		 * 
		 * @remarks
		 * The channel info object is a read only snapshot of the
		 * channel, and will not be updated automatically.
		 * To get current information, you need to continuously retrieve
		 * new channel info objects. But don't forget to destroy the old
		 * ones, otherwise you will cause memory leakage.
		 * 
		 * @return
		 * RES_OK on success, an error code otherwise.
		 */
		Result cacheCreateChannelInfoFromChannelName(void *network,
							const int8 *channel,
							const void **channelInfo)
		{
			Result res = RES_INVALID_PARAMETER;
			
			if ( NULL != network
				&& NULL != channel )
			{
				res = ((NetworkManager *)network)->getChannelInfo(
					network, channel, channelInfo);
			}
			
			return res;
		}
		
		/**
		 * @brief
		 * Deallocates a channel info object.
		 * 
		 * @param channelInfo
		 * ChannelInfo object to destroy.
		 * 
		 * @sa
		 * cacheCreateChannelInfoFromChannelName
		 */
		void cacheDestroyChannelInfo(void *channelInfo)
		{
			/// @todo Move deletion into ircnetworkcache,
			/// cast to (Channel *)
			if ( NULL != channelInfo )
			{
				delete channelInfo;
			}
		}
	}
} // namespace firc
} // namespace anp
