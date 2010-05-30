#ifndef _NETWORKMANAGER_H_
#define _NETWORKMANAGER_H_

#include <basedefs.h>
#include <tcpconnection.h>
#include <networkcache.h>
#include "inetwork.h"
#include "inc/messagesender.h"
#include <eventdispatcher.h>
#include <networkevents.h>
#include <log_singleton.h>

#include <anp_threading.h> // hide?
#include <memory>

namespace anp
{
namespace firc
{
	class PluginManager;
	class MsgPrefix;
	
	class Network: public INetwork
	{
	public:
		Network(const int8 *host, const int8 *port);
		~Network();
		
		void deinit(const std::string &message);
		enum State
		{
			CONNECTING=0,
			REGISTERING,
			CONNECTED,
			SHUTTING_DOWN,
			UNKNOWN
		};
		// Non-blocking, creates a thread on it's own
		void runMessageReceiverInThread();
		// Blocking
		void runMessageReceiver();
		
		void sendMessage(const std::string &message);
		
		// Network cache stuff
		const NetworkCacheUserInterface &networkCache() const;
		Result getBotNickName(int8 *destName,
								uint32 maxNameLength) const;

		// Event stuff
		dispatchers::Join &
			eventDispatcherJoin();

		dispatchers::Part &
			eventDispatcherPart();

		dispatchers::PrivMsg &
			eventDispatcherPrivMsg();

		dispatchers::Topic &
			eventDispatcherTopic();

		dispatchers::NumericReply &
			eventDispatcherNumericReply();
			
		dispatchers::Ping &
			eventDispatcherPing();

	private:
		void parseMessage(const std::string &message);
		void msgPingHandle( const MsgPrefix &origin,
							const std::string &server1,
							const std::string &server2);
		void msgJoinHandle(	const MsgPrefix &origin,
							const std::string &channel);
		void msgPartHandle(	const MsgPrefix &origin,
							const std::string &channel,
							const std::string &message);
		void msgPrivMsgHandle(	const MsgPrefix &origin,
								const std::string &target,
								const std::string &message);
		void msgTopicHandle(const MsgPrefix &origin,
							const std::string &channel,
							const std::string &topic);
		void msgNumHandleRPL_NAMREPLY(
								const std::string &channel,
								const std::string &userlist);
		void msgNumHandle(const MsgPrefix &origin,
							const std::string &command,
							const std::string params[]);

		State m_state;
		threading::Mutex m_stateMutex;
		
		TCPConnection m_connection;
		
		std::auto_ptr<threading::Thread> m_receiverThread;
		std::auto_ptr<MessageSender> m_messageSender;
		
		NetworkCache m_networkCache;

		struct
		{
			EventDispatcher<
				ISubscriber<events::Join>,
				events::Join
			> join;
			EventDispatcher<
				ISubscriber<events::Part>,
				events::Part
			> part;
			EventDispatcher<
				ISubscriber<events::PrivMsg>,
				events::PrivMsg
			> privMsg;
			EventDispatcher<
				ISubscriber<events::Topic>,
				events::Topic
			> topic;
			EventDispatcher<
				ISubscriber<events::NumericReply>,
				events::NumericReply
			> num;
			EventDispatcher<
				ISubscriber<events::Ping>,
				events::Ping
			> ping;
		} m_eventDispatchers;
		
		LogSingletonHelper m_log;
	};
} // namespace firc
} // namespace anp

#endif // _NETWORKMANAGER_H_
