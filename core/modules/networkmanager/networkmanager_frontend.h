#ifndef _NETWORKMANAGER_FRONTEND_H_
#define _NETWORKMANAGER_FRONTEND_H_

#include <string>
#include <eventdispatcher.h>
#include <networkmanagerevents.h>

namespace anp
{
namespace firc
{
	class NetworkCacheUserInterface;

	class INetworkManagerFrontend
	{
	public:
		INetworkManagerFrontend() { }
		virtual ~INetworkManagerFrontend() { }

		virtual void deinit(const std::string &message) = 0;

		virtual void sendMessage(const std::string &message) = 0;

		virtual void runMessageReceiverInThread() = 0;
		
		// Network cache stuff (getCache() instead ?)

		virtual const NetworkCacheUserInterface &networkCache() const = 0;

		virtual IEventDispatcherSubscriber
		<events::ISubscriber<events::Join> > &
		eventDispatcherJoin() = 0;

		virtual IEventDispatcherSubscriber
		<events::ISubscriber<events::Part> > &
		eventDispatcherPart() = 0;

		virtual IEventDispatcherSubscriber
		<events::ISubscriber<events::PrivMsg> > &
		eventDispatcherPrivMsg() = 0;

		virtual IEventDispatcherSubscriber
		<events::ISubscriber<events::Topic> > &
		eventDispatcherTopic() = 0;

		virtual IEventDispatcherSubscriber
		<events::ISubscriber<events::NumericReply> > &
		eventDispatcherNumericReply() = 0;
	};
}
}

#endif // _NETWORKMANAGER_FRONTEND_H_
