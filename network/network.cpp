/*

Copyright (c) 2010, Anton Petersson < anton (at) anp.nu >
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "network.h"
#include <tokenizer.h>
#include <tcpconnection.h>
#include <messageprefix.h>

#include <sstream>
#include <stdexcept>
#include <memory>
#include <pcrecpp.h>

namespace anp
{
namespace irc
{
namespace numeric_replies
{
	const int8 *const RPL_NAMREPLY = "353";
}
	void *Network::threadRunMessageReceiver(void *arg)
	{
		Network *nm = (Network *)arg;
		if ( NULL != nm )
		{
			try
			{
				nm->runMessageReceiver();
			} catch ( std::exception &e )
			{
				std::stringstream ss;
				ss << "Network-MessageReceiverRunner: Exception occured: "
				   << e.what() << std::endl;
				ANPLOGE("libfirc", ss.str());
				
				events::ExceptionOccured event(e);
				nm->m_eventDispatchers.exceptionOccured.dispatch(event);
			}
		} else
		{
			ANPLOGE("libfirc", "Network: threadRunMessageReceiver Network == NULL");
		}
		ANPLOGD("libfirc", "Network: closing thread");
		pthread_exit(0);
	}
	
	Network::Network(const int8 *host, const int8 *port,
					 const std::string &nick, const std::string &user,
					 const std::string &realName):
	m_state(CONNECTING),
	m_connection(host, port),
	m_host(host),
	m_port(port),
	m_messageSender(new MessageSender(m_connection))
	{
		std::string m_outStr;
		
		ANPLOGI("libfirc", "Successfully connected.");
		
		m_state = REGISTERING;
		// Nick
		m_outStr = std::string("NICK ")
			+nick
			+std::string("\r\n");
		m_connection.send(m_outStr);
		// User
		m_outStr = std::string("USER ")
			+user
			+std::string(" 0 * :")
			+realName
			+std::string("\r\n");
		m_connection.send(m_outStr);
		
		m_networkCache.setClientNickName(nick);
		m_eventDispatchers.num.subscribe(&m_networkCache);
		m_eventDispatchers.join.subscribe(&m_networkCache);
		m_eventDispatchers.part.subscribe(&m_networkCache);
		m_eventDispatchers.topic.subscribe(&m_networkCache);
		
		m_state = CONNECTED;
	}
	
	Network::~Network()
	{
		if ( NULL != m_receiverThread.get() )
		{
			// Ask the thread to exit
			m_stateMutex.lock();
			m_state = SHUTTING_DOWN;
			m_stateMutex.unlock();
			// Now just wait for it to finnish
			m_receiverThread->join(NULL);
			// (Implicitly) kill the connection
		}
	}
	
	/**
	Sends the QUIT message to the server and waits (blocking)
	for the receiver thread to finish.

	You still need to destroy the Network object
	after calling this function.
	*/
	void Network::deinit(const std::string &message)
	{
		m_stateMutex.lock();
		m_state = SHUTTING_DOWN;
		m_stateMutex.unlock();
		
		std::string quitMessage("QUIT :");
		quitMessage += message;
		quitMessage += "\r\n";
		m_connection.send(quitMessage);
		
		if ( NULL != m_receiverThread.get() )
		{
			m_receiverThread->join(NULL);
			m_receiverThread.reset(NULL);
		}
	}

	/**
	Creates a thread to be able to run the message receiver
	without blocking.

	Throws an exception if the thread cannot be created.
	*/
	void Network::runMessageReceiverInThread()
	{
		m_receiverThread.reset(new threading::Thread);
		m_receiverThread->create(NULL,	threadRunMessageReceiver,
								(void *)this);
	}
	
	/**
	Runs the message receiver, doesn't return until disconnected.
	*/
	void Network::runMessageReceiver()
	{
		using tokenizer::tokenize;
		State state = UNKNOWN;
		bool32 connected = true;

		char buffer[MAX_DATA_LENGTH];
		std::string in,
					currentMessage,
					leftOvers;
		std::stringstream ss;
		
		anp::threading::TryLock lock(m_runMutex);

		while ( TRUE == connected )
		{			
			m_stateMutex.lock();
			state = m_state;
			m_stateMutex.unlock();
			
			if ( state == SHUTTING_DOWN )
			{
				break;
			}

			// 1. Receive a message
			if ( m_connection.waitForSocket(0, 500000) )
			{
				m_connection.receive(buffer, sizeof(buffer)/sizeof(buffer[0]));
			} else
			{
				// There was no data available from the server
				// check again
				continue;
			}
			in = buffer;
			// Tokenize it and parse it
			// Start by dividing it up into several messages
			while ( 1 ) {
				if ( tokenize(currentMessage, in, "\r\n") ) {
					// if there was something left from the previous
					// parsing run, add it here
					if ( leftOvers.length() > 0 ) {
						currentMessage = leftOvers + currentMessage;
						leftOvers.erase();
					}
					ss << "<- " << currentMessage;
					ANPLOGD("libfirc", ss.str());
					ss.str("");
					parseMessage(currentMessage);
				} else {
					// The message isn't complete, so save it and
					// add the rest when it arrives
                    leftOvers += currentMessage;
					break;
				}
				if ( in.length() == 0 )
					break;
			}			
		}
	}

	void parseParams(std::string &all, std::string list[])
	{
		using tokenizer::tokenize;
		// Assuming list holds 15 elements

		for ( int i=0; i<15; i++ )
		{
			if ( all[0] == ':' )
			{
				list[i] = all.substr(1);
				break;
			} else if ( !tokenize(list[i], all, " ") )
			{
				break;
			}
		}
	}

	void Network::parseMessage(const std::string &message)
	{
		using pcrecpp::RE;

		std::string prefix, command, parameters, params[15];
		std::string nick, user, host;

		static
		RE ircMsgPattern(
			"^(:(\\S+?)\\s)?(\\S+)\\s?(:?.*)$"
		);
		static
		RE prefixPattern("^(\\S+?)!(\\S+?)@(\\S+?)$");
		static
		RE numPattern("^[[:digit:]]{3}$");

		bool32 validMessage = ircMsgPattern.FullMatch(message, (void *)0,
			&prefix, &command, &parameters);

		if ( validMessage )
		{
			bool32 validUser = prefixPattern.FullMatch(prefix,
														&nick,
														&user,
														&host);
			MsgPrefix msgPrefix(prefix, nick, user, host);

			parseParams(parameters, params);

			if ( numPattern.FullMatch(command) )
			{
				// Numeric replies
				if ( command == numeric_replies::RPL_NAMREPLY )
				{
					msgNumHandleRPL_NAMREPLY(params[2], params[3]);
				}
				msgNumHandle(msgPrefix, command, params);
			} else
			{
				if ( command == "PING" )
				{
					msgPingHandle(msgPrefix, params[0], "");
				} else if ( command == "JOIN" )
				{
					msgJoinHandle(msgPrefix, params[0]);
				} else if ( command == "PART" )
				{
					msgPartHandle(msgPrefix, params[0],
									params[1]);
				} else if ( command == "PRIVMSG" )
				{
					msgPrivMsgHandle(msgPrefix, params[0],
										params[1]);
				} else if ( command == "TOPIC" )
				{
					msgTopicHandle(msgPrefix, params[0],
										params[1]);
				}
				msgCommandHandle(msgPrefix, command, params);
			}

		} else
		{
			std::stringstream ss;
			ss << "(lib)Invalid IRC message: " << message
				<< "(" << "p=" << prefix << "c=" << command << "pa="
				<< parameters << ")";
			ANPLOGD("libfirc", ss.str());
			// unknownMessageHandler()!
		}
	}

	void Network::msgPingHandle(const MsgPrefix &origin,
								const std::string &server1,
								const std::string &server2)
	{
		std::string pong = "PONG :";
		pong += server1 + "\r\n";

		m_connection.send(pong);
		
		events::Ping event(*this, origin, server1, server2);
		m_eventDispatchers.ping.dispatch(event);
	}

	void Network::msgJoinHandle( const MsgPrefix &origin,
										const std::string &channel)
	{
		events::Join event(*this, origin, channel);
		m_eventDispatchers.join.dispatch(event);
	}

	void Network::msgPartHandle(	const MsgPrefix &origin,
										const std::string &channel,
										const std::string &message)
	{
		events::Part event(*this, origin, channel, message);
		m_eventDispatchers.part.dispatch(event);
	}
	
	void Network::msgPrivMsgHandle(	const MsgPrefix &origin,
											const std::string &target,
											const std::string &message)
	{
		events::PrivMsg event(*this, origin, target, message);
		m_eventDispatchers.privMsg.dispatch(event);
	}
	
	void Network::msgTopicHandle(const MsgPrefix &origin,
										const std::string &channel,
										const std::string &topic)
	{
		events::Topic event(*this, origin, channel, topic);
		m_eventDispatchers.topic.dispatch(event);
	}

	void Network::msgNumHandleRPL_NAMREPLY(
								const std::string &channel,
								const std::string &userlist)
	{
		using tokenizer::tokenize;
		std::string nick, userlistCopy = userlist;
		bool keepGoing = true;

		while ( keepGoing ) {
			// nick = [@|+|nothing]nickname
			keepGoing = tokenize(nick, // note, reusing var nick
								 userlistCopy,
								 " ");
			switch ( nick[0] ) {
			case '@':
				nick.erase(0, 1);
				break;
			case '+':
				nick.erase(0, 1);
				break;
			default:
				break;
			}
			// Stupid temporary fix.
			// Parsing should be fixed instead.
			if ( nick != "" && nick != " " )
			{
				m_networkCache.addUserToChannel(nick, "", "",
												 channel);
			}
		}
	}

	void Network::msgNumHandle(const MsgPrefix &origin,
							   const std::string &command,
							   const std::string params[])
	{
		events::NumericReply event(*this, origin, command, params);
		m_eventDispatchers.num.dispatch(event);
	}
	
	void Network::msgCommandHandle(const MsgPrefix &origin,
								   const std::string &command,
								   const std::string params[])
	{
		events::Command event(*this, origin, command, params);
		m_eventDispatchers.command.dispatch(event);
	}
	
	void Network::sendMessage(const std::string &message)
	{
		m_messageSender->addMessage(message);
	}

	const std::string &Network::host()
	{
		return m_host;
	}
	
	const std::string &Network::port()
	{
		return m_port;
	}

	const NetworkCacheUserInterface &Network::networkCache() const
	{
		return m_networkCache;
	}

	dispatchers::Join &
	Network::eventDispatcherJoin()
	{
		return m_eventDispatchers.join;
	}

	dispatchers::Part &
	Network::eventDispatcherPart()
	{
		return m_eventDispatchers.part;
	}

	dispatchers::PrivMsg &
	Network::eventDispatcherPrivMsg()
	{
		return m_eventDispatchers.privMsg;
	}

	dispatchers::Topic &
	Network::eventDispatcherTopic()
	{
		return m_eventDispatchers.topic;
	}

	dispatchers::NumericReply &
	Network::eventDispatcherNumericReply()
	{
		return m_eventDispatchers.num;
	}
	
	dispatchers::Command &
	Network::eventDispatcherCommand()
	{
		return m_eventDispatchers.command;
	}
	
	dispatchers::Ping &
	Network::eventDispatcherPing()
	{
		return m_eventDispatchers.ping;
	}
	
	dispatchers::ExceptionOccured &
	Network::eventDispatcherExceptionOccured()
	{
		return m_eventDispatchers.exceptionOccured;
	}

} // namespace irc
} // namespace anp
