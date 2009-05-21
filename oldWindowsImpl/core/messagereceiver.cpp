#include "messagereceiver.h"
#include "pluginmanager.h"
#include "tcpconnection.h"
#include "tokenizer.h"
#include "ircnetworkcache.h"

CMessageReceiver::CMessageReceiver(CTCPConnection *connection, CEventProcessor *eventProcessor, /*CMessageSender *messageSender*/CPluginManager *pluginManager, int agentID, irc::NetworkCache *cache):
#ifdef _DEBUG
CActiveObject("CMessageReceiver"),
#endif
m_connection(connection),
m_eventProcessor(*eventProcessor),
//m_pluginManager(*messageSender),
m_pluginManager(pluginManager),
m_agentID(agentID),
m_cache(cache) {

	// Start executing!
	m_thread.resume();
}

void CMessageReceiver::initThread() {
#ifdef _DEBUG
	::SetThreadName(-1, "CMessageReceiver");
#endif
}

void CMessageReceiver::run() {
	using tokenizer::tokenize;

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
	while ( !m_isDying ) {
		// 1. Receive a message
		if ( N_CONNECTION_CLOSED == m_connection->Receive(buffer) ) {
			// The server closed the connection, what to do?
			// Well it's unnessecary for us to wait for messages
			m_isDying = 1;
			m_eventProcessor.addEvent(m_agentID, -1, -1, "-- Disconnected from server, by server --\n");
			return;
			// Perhaps send some stupid event or something
		}
		in = buffer; // optimize..?
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
				m_eventProcessor.addEvent(m_agentID, -1, -1, s2.str());

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
					m_pluginManager->onPing(currentMessage);
				} else if ( command == "PRIVMSG" ) {
					tokenize(target, currentMessage, ' ');
//					m_pluginManager->onPrivMsg(

					//std::stringstream s;
					//s << "PRIVMSG DETECTED: [Prefix] '" << prefix << "' [target] '" << target << "' [msg] '" << currentMessage.erase(0, 1) << "'\n";
					//m_eventProcessor.addEvent(m_agentID, -1, -1, s.str());
					//m_eventProcessor.addEvent(m_agentID, -1, -1, s.str());
					//m_eventProcessor.addEvent(m_agentID, -1, -1, s.str());
					currentMessage.erase(0, 1);

					m_pluginManager->onPrivMsg(prefix, target, currentMessage);
				} else if ( command == "JOIN" ) {
					if ( currentMessage[0] == ':' )
						currentMessage.erase(0, 1); // Remove ':' in front of the channel name (efnet seems to use it)

					// Separate the prefix into nickname, user and host
					tokenize(temp1, prefix, '!'); // nickname
					tokenize(temp2, prefix, '@'); // user
					// prefix now only holds the host

					m_cache->onJoin(temp1, temp2, prefix, currentMessage);
					m_pluginManager->onJoin(temp1, currentMessage);
				} else if ( command == "PART" ) {
					if ( currentMessage[0] == ':' )
						currentMessage.erase(0, 1); // Remove ':' in front of the channel name

					// Separate the prefix into nickname, user and host (UNNECESSARY FOR PART MESSAGE !? ONLY USES NICKNAME ANYWAY!!!)
					tokenize(temp1, prefix, '!'); // nickname
					tokenize(temp2, prefix, '@'); // user
					// prefix now only holds the host

					m_cache->onPart(temp1, temp2, prefix, currentMessage);
					m_pluginManager->onPart(temp1, currentMessage);
				} else if ( command == "TOPIC" ) {
					// topic change.. ex: (GTANet) [Original message] :Pliskin!IceChat7@gtanet-ADE03D4F.bsnet.se TOPIC #antons_kanal :Det här är en topic
					tokenize(target, currentMessage, " :");

					m_cache->setTopic(target, currentMessage);
					//m_pluginManager->onTopicChange(); TODO

					// ##### NUMERIC REPLIES BELOW ######
				} else if ( command == "332" ) {
					// RPL_TOPIC ex: (GTANet) [Original message] :frosties.de.eu.gtanet.com 332 firc_04 #antons_kanal :lalala
					tokenize(target, currentMessage, ' '); // target = firc_04
					tokenize(temp1, currentMessage, " :"); // temp1 = #antons_kanal
					
					m_cache->setTopic(temp1, currentMessage);
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
						m_cache->addUser(temp2, temp3);
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
	}
}

void CMessageReceiver::flushThread() {
	// perhaps not here, think about it later (unsure)
	m_connection->Send("QUIT :leaving.\r\n");

	// close the connection here or in the destructor? leaning towards here, since I can't trust the server will close the connection)
}