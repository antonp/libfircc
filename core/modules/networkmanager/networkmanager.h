#ifndef _NETWORKMANAGER_H_
#define _NETWORKMANAGER_H_

#include <basedefs.h>
#include <tcpconnection.h>
#include <networkcache.h>
#include "networkmanager_frontend.h"
#include "inc/messagesender.h"

#include <anp_threading.h> // johnny bigert?
#include <memory>

namespace anp
{
namespace firc
{
	class PluginManager;
	class MsgPrefix;
	
	class NetworkManager: public INetworkManagerFrontend
	{
	public:
		NetworkManager(const int8 *host, const int8 *port,
						PluginManager *m_pluginManager);
		~NetworkManager();
		
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
		Result runMessageReceiver();
		
		void sendMessage(const std::string &message);
		
		// Network cache stuff
		const NetworkCacheUserInterface &networkCache() const;
		Result getBotNickName(int8 *destName,
								uint32 maxNameLength) const;
		Result getTopic(const int8 *channel, int8 *destTopic,
						uint32 maxTopicSize) const;
		Result getChannelUserCount(const int8 *channel,
									uint32 *destChannelUserCount) const;
		Result isUserInChannel(const int8 *channel,
								const int8 *nickName,
								bool32 *userIsInChannel) const;
									
		Result getChannelInfo(void *network, const int8 *channel,
								const void **userList) const;
	private:
		void parseMessage(const std::string &message);
		void msgPingHandle(const std::string &server1,
							const std::string &server2);
		void msgJoinHandle(const std::string &channel,
							const std::string &nick,
							const std::string &user,
							const std::string &host);
		void msgPartHandle(const std::string &channel,
							const std::string &nick,
							const std::string &user,
							const std::string &host);
		void msgPrivMsgHandle(	const MsgPrefix &origin,
								const std::string &target,
								const std::string &message);
		void msgTopicHandle(const MsgPrefix &origin,
							const std::string &channel,
							const std::string &topic);
	
		State m_state;
		//pthread_mutex_t m_stateMutex;
		threading::Mutex m_stateMutex;
		
		TCPConnection m_connection;
		
		std::auto_ptr<threading::Thread> m_receiverThread;
		std::auto_ptr<MessageSender> m_messageSender;
		
		PluginManager *m_pluginManager;
		NetworkCache m_networkCache;
	};
} // namespace firc
} // namespace anp

#endif // _NETWORKMANAGER_H_
