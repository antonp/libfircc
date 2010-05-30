#ifndef _NETWORKMANAGEREVENTS_H_
#define _NETWORKMANAGEREVENTS_H_

#include <basedefs.h>
#include <eventdispatcher.h>
#include <messageprefix.h>

namespace anp
{
namespace firc
{
class INetwork;
namespace events
{

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
	INetwork &network() { return m_network; }
	const MsgPrefix &origin() const { return m_origin; }
protected:
	INetwork &m_network;
	const MsgPrefix m_origin;
};

class EventWithChannel
{
public:
	EventWithChannel(const std::string &channel): m_channel(channel)
	{
	}
	virtual ~EventWithChannel() { }
	const std::string &channel() const { return m_channel; }
protected:
	std::string m_channel;
};

class EventWithTarget
{
public:
	EventWithTarget(const std::string &target):
	m_target(target)
	{
	}
	virtual ~EventWithTarget() { }
	const std::string &target() const { return m_target; }
protected:
	std::string m_target;
};

class EventWithMessage
{
public:
	EventWithMessage(const std::string &message): m_message(message)
	{
	}
	virtual ~EventWithMessage() { }
	const std::string &message() const { return m_message; }
protected:
	std::string m_message;
};

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
	const std::string &param(uint32 index) const
	{
		return m_params[index];
	}
protected:
	std::string m_params[15];
};

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
	const std::string &topic() const { return m_topic; }
protected:
	std::string m_topic;
};

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
	const std::string &server1() const { return m_server1; }
	const std::string &server2() const { return m_server2; }
protected:
	std::string m_server1;
	std::string m_server2;
};

class NewNetwork
{
public:
	NewNetwork(anp::firc::INetwork &network):
	m_network(network)
	{
	}

	anp::firc::INetwork &network()
	{
		return m_network;
	}
protected:
	anp::firc::INetwork &m_network;
};

class RemovingNetwork
{
public:
	RemovingNetwork(anp::firc::INetwork &network):
	m_network(network)
	{
	}

	anp::firc::INetwork &network()
	{
		return m_network;
	}
protected:
	anp::firc::INetwork &m_network;
};

} // namespace events
} // namespace firc
} // namespace anp

#endif // _NETWORKMANAGEREVENTS_H_
