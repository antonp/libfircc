#ifndef _NETWORKMANAGEREVENTS_H_
#define _NETWORKMANAGEREVENTS_H_

#include <eventdispatcher.h>
#include <messageprefix.h>

namespace anp
{
namespace firc
{
class INetworkManagerFrontend;
namespace events
{

class IRCEvent
{
public:
	IRCEvent(INetworkManagerFrontend &network,
			const MsgPrefix &origin):
	m_network(network),
	m_origin(origin)
	{
	}
	virtual ~IRCEvent() { }
	INetworkManagerFrontend &network() { return m_network; }
	const MsgPrefix &origin() const { return m_origin; }
protected:
	INetworkManagerFrontend &m_network;
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

class Join: public IRCEvent, public EventWithChannel
{
public:
	Join(INetworkManagerFrontend &network,
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
	Part(INetworkManagerFrontend &network,
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
	PrivMsg(INetworkManagerFrontend &network,
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
	Topic(INetworkManagerFrontend &network,
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

template <typename E>
class ISubscriber
{
public:
	virtual ~ISubscriber() { }
	virtual void receiveEvent(E &event) = 0;
};

} // namespace events
} // namespace firc
} // namespace anp

#endif // _NETWORKMANAGEREVENTS_H_
