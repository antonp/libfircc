#ifndef _NETWORKMANAGER_FRONTEND_H_
#define _NETWORKMANAGER_FRONTEND_H_

#include <string>
#include <eventdispatcher.h>
#include <networkeventdispatchers.h>
#include <networkevents.h>

namespace anp
{
namespace firc
{	
	class NetworkCacheUserInterface;

	class INetwork
	{
	public:
		INetwork() { }
		virtual ~INetwork() { }

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
		virtual dispatchers::Join &
		eventDispatcherJoin() = 0;

		/**
		Returns a subscriber interface for the PART event dispatcher.
		*/
		virtual dispatchers::Part &
		eventDispatcherPart() = 0;

		/**
		Returns a subscriber interface for the PRIVMSG event dispatcher.
		*/
		virtual dispatchers::PrivMsg &
		eventDispatcherPrivMsg() = 0;

		/**
		Returns a subscriber interface for the TOPIC event dispatcher.
		*/
		virtual dispatchers::Topic &
		eventDispatcherTopic() = 0;

		/**
		Returns a subscriber interface for the numeric reply event
		dispatcher, which is a generic dispatcher for all numeric
		replies.
		*/
		virtual dispatchers::NumericReply &
		eventDispatcherNumericReply() = 0;

		/**
		Returns a subscriber interface for the PING event dispatcher.
		
		@remark 
		No response is needed since the INetwork implementation should
		respond to this message automatically.
		*/		
		virtual dispatchers::Ping &
		eventDispatcherPing() = 0;
	};
}
}

#endif // _NETWORKMANAGER_FRONTEND_H_
