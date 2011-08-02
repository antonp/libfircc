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

#ifndef _NETWORKMANAGEREVENTS_H_
#define _NETWORKMANAGEREVENTS_H_

#include <anpcode/basedefs.h>
#include <anpcode/eventdispatcher.h>
#include <fircc/messageprefix.h>

namespace anp
{
namespace irc
{
class INetwork;
namespace events
{

/**
 * Base IRC event class.
 */
class IRCEvent
{
public:
	IRCEvent(INetwork &network,
			const MsgPrefix &origin):
	m_network(network),
	m_origin(origin)
	{
	}
	virtual ~IRCEvent() { }
	
	/**
	 * @return
	 * A network interface.
	 */
	INetwork &network() { return m_network; }
	
	/**
	 * @return
	 * The origin of the event.
	 */
	const MsgPrefix &origin() const { return m_origin; }
protected:
	INetwork &m_network;
	const MsgPrefix m_origin;
};

/**
 * Base class for all events that includes a channel.
 */
class EventWithChannel
{
public:
	EventWithChannel(const std::string &channel): m_channel(channel)
	{
	}
	virtual ~EventWithChannel() { }
	
	/**
	 * @return
	 * The channel.
	 */
	const std::string &channel() const { return m_channel; }
protected:
	std::string m_channel;
};

/**
 * Base class for all events that has a target. The target is typically a
 * channel or a user.
 */
class EventWithTarget
{
public:
	EventWithTarget(const std::string &target):
	m_target(target)
	{
	}
	virtual ~EventWithTarget() { }
	
	/**
	 * @return
	 * The target.
	 */
	const std::string &target() const { return m_target; }
protected:
	std::string m_target;
};

/**
 * Base class for all events with a message.
 */
class EventWithMessage
{
public:
	EventWithMessage(const std::string &message): m_message(message)
	{
	}
	virtual ~EventWithMessage() { }
	
	/**
	 * @return
	 * The message.
	 */
	const std::string &message() const { return m_message; }
protected:
	std::string m_message;
};

/**
 * Base class for events with a command.
 */
class EventWithCommand
{
public:
	EventWithCommand(const std::string &command): m_command(command)
	{
	}
	virtual ~EventWithCommand() { }
	const std::string &command() const { return m_command; }
protected:
	std::string m_command;
};

/**
 * Base class for events with a list of parameters.
 */
class EventWithParamList
{
public:
	EventWithParamList(const std::string params[])
	{
		for ( uint32 i=0; i<15; ++i )
		{
			m_params[i] = params[i];
		}
	}
	virtual ~EventWithParamList() { }
	
	/**
	 * @param index
	 * Index of the parameter to retrieve.
	 * 
	 * @return
	 * The requested parameter.
	 */
	const std::string &param(uint32 index) const
	{
		return m_params[index];
	}
protected:
	std::string m_params[15];
};

/**
 * The join event is sent whenever the client is notified of a user entering a
 * channel. This usually only happens when the client is in that particular
 * channel. The event contains information like nick name of the joining user
 * and name of the channel.
 */
class Join: public IRCEvent, public EventWithChannel
{
public:
	Join(INetwork &network,
		const MsgPrefix &origin,
		const std::string &channel):
	IRCEvent(network, origin),
	EventWithChannel(channel)
	{
	}
};

/**
 * The part event is sent whenever the client is notified of a user leaving a
 * channel. This usually only happens when the client is in that particular
 * channel. For this event, the origin is the user leaving the channel. It also
 * contains the name of the channel and the optional message.
 */
class Part: public IRCEvent,
			public EventWithChannel,
			public EventWithMessage
{
public:
	Part(INetwork &network,
		const MsgPrefix &origin,
		const std::string &channel,
		const std::string &message):
	IRCEvent(network, origin),
	EventWithChannel(channel),
	EventWithMessage(message)
	{
	}
};

/**
 * This event is sent whenever the client receives a PRIVMSG from the server.
 * For this event, the origin is the user who sent the message. The target is
 * the user or channel (every user in that channel) receiving the message.
 */
class PrivMsg: public IRCEvent,
				public EventWithTarget,
				public EventWithMessage
{
public:
	PrivMsg(INetwork &network,
			const MsgPrefix &origin,
			const std::string &target,
			const std::string &message):
	IRCEvent(network, origin),
	EventWithTarget(target),
	EventWithMessage(message)
	{
	}
};

/**
 * This event is sent whenever the client receives the TOPIC message from the
 * server. This indicates that the topic for a particular channel has changed.
 * For this event, the origin is the user who changed the topic.
 */
class Topic: public IRCEvent,
			public EventWithChannel
{
public:
	Topic(INetwork &network,
			const MsgPrefix &origin,
			const std::string &channel,
			const std::string &topic):
	IRCEvent(network, origin),
	EventWithChannel(channel),
	m_topic(topic)
	{
	}
	
	/**
	 * @return
	 * The new topic.
	 */
	const std::string &topic() const { return m_topic; }
protected:
	std::string m_topic;
};

/**
 * This is a generic event which is sent everytime the client receives a
 * numeric reply from the server. Most IRC messages/commands generate numeric
 * replies. The complete list of numeric replies can be found in rfc2812.
 * For this event, the command is the numeric reply.
 */
class NumericReply: public IRCEvent,
					public EventWithCommand,
					public EventWithParamList
{
public:
	NumericReply(INetwork &network,
					const MsgPrefix &origin,
					const std::string &command,
					const std::string params[]):
	IRCEvent(network, origin),
	EventWithCommand(command),
	EventWithParamList(params)
	{
	}
};

/**
 * This is a generic event which is sent everytime the client receives a
 * valid irc command.
 */
class Command: public IRCEvent,
			   public EventWithCommand,
			   public EventWithParamList
{
public:
	Command(INetwork &network,
			const MsgPrefix &origin,
			const std::string &command,
			const std::string params[]):
	IRCEvent(network, origin),
	EventWithCommand(command),
	EventWithParamList(params)
	{
	}
};

/**
 * This event is sent everytime the client receives a PING message form the
 * server. The client should reply with a PONG message which is currently
 * handled internally in the library.
 */
class Ping: public IRCEvent
{
public:
	Ping(INetwork &network,
		const MsgPrefix &origin,
		const std::string &server1,
		const std::string &server2):
	IRCEvent(network, origin),
	m_server1(server1),
	m_server2(server2)
	{
	}
	
	/**
	 * @return
	 * Server 1.
	 */
	const std::string &server1() const { return m_server1; }
	
	/**
	 * @return
	 * Server 2.
	 */
	const std::string &server2() const { return m_server2; }
protected:
	std::string m_server1;
	std::string m_server2;
};

/**
 * This event is typically sent from a NetworkFactory when a new Network has
 * been opened (successfully connected and registered to).
 * This event should be used to subscribe to events for the new network.
 */
class NewNetwork
{
public:
	NewNetwork(anp::irc::INetwork &network):
	m_network(network)
	{
	}

	/**
	 * @return
	 * Interface to the new network.
	 */
	anp::irc::INetwork &network()
	{
		return m_network;
	}
protected:
	anp::irc::INetwork &m_network;
};

/**
 * This event is typically sent from a NetworkFactory prior to removing it.
 * Upon receiving this event, subscribers should unsubscribe and make sure
 * they throw away all references to the network.
 */
class RemovingNetwork
{
public:
	RemovingNetwork(anp::irc::INetwork &network):
	m_network(network)
	{
	}

	/**
	 * @return
	 * Interface to the network about to be removed.
	 */
	anp::irc::INetwork &network()
	{
		return m_network;
	}
protected:
	anp::irc::INetwork &m_network;
};

/**
 * This event is sent when an exception has occured in a internal thread of the
 * library.
 */
class ExceptionOccured
{
public:
	ExceptionOccured(std::exception &e)
	{
		m_exception = e;
	}
	
	/**
	 * @return
	 * The exception.
	 */
	std::exception &exception()
	{
		return m_exception;
	}
protected:
	std::exception m_exception;
};

} // namespace events
} // namespace irc
} // namespace anp

#endif // _NETWORKMANAGEREVENTS_H_
