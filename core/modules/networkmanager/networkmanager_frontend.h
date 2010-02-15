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

		/**
		Sends the QUIT message to IRC and blocks until the receiver
		thread (if any) has exited.

		@param message
		The quit message to send to the IRC network.
		*/
		virtual void deinit(const std::string &message) = 0;

		/**
		Sends a raw message to the IRC server.

		@param message
		The message to send. Should include the trailing CRLF as
		it is not added automatically by this method.
		*/
		virtual void sendMessage(const std::string &message) = 0;

		/**
		Spawns a thread that calls runMessageReceiver.
		*/
		virtual void runMessageReceiverInThread() = 0;
		
		// Network cache stuff (getCache() instead ?)

		/**
		Returns a reference to a NetworkCacheUserInterface which
		can be used to query the internal database for information
		about channels and users, but not to modify it.
		*/
		virtual const NetworkCacheUserInterface &networkCache() const = 0;

		/**
		Returns a subscriber interface for the JOIN event dispatcher.
		*/
		virtual IEventDispatcherSubscriber
		<events::ISubscriber<events::Join> > &
		eventDispatcherJoin() = 0;

		/**
		Returns a subscriber interface for the PART event dispatcher.
		*/
		virtual IEventDispatcherSubscriber
		<events::ISubscriber<events::Part> > &
		eventDispatcherPart() = 0;

		/**
		Returns a subscriber interface for the PRIVMSG event dispatcher.
		*/
		virtual IEventDispatcherSubscriber
		<events::ISubscriber<events::PrivMsg> > &
		eventDispatcherPrivMsg() = 0;

		/**
		Returns a subscriber interface for the TOPIC event dispatcher.
		*/
		virtual IEventDispatcherSubscriber
		<events::ISubscriber<events::Topic> > &
		eventDispatcherTopic() = 0;

		/**
		Returns a subscriber interface for the numeric reply event
		dispatcher, which is a generic dispatcher for all numeric
		replies.
		*/
		virtual IEventDispatcherSubscriber
		<events::ISubscriber<events::NumericReply> > &
		eventDispatcherNumericReply() = 0;
	};
}
}

#endif // _NETWORKMANAGER_FRONTEND_H_
