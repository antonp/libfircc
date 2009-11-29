#ifndef _NETWORKMANAGER_H_
#define _NETWORKMANAGER_H_

#include <basedefs.h>
#include <tcpconnection.h>
#include <ircnetworkcache.h>
#include "inc/messagesender.h"

#include <anp_threading.h> // johnny bigert?
#include <memory>

namespace anp
{
namespace firc
{
	class PluginManager;
	
	class NetworkManager
	{
	public:
		NetworkManager();
		~NetworkManager();
		
		Result init(const int8 *host, const int8 *port,
					PluginManager *m_pluginManager);
		void deinit(const int8 *message);
		enum State
		{
			CONNECTING=0,
			REGISTERING,
			CONNECTED,
			SHUTTING_DOWN,
			UNKNOWN
		};
		// Should be private (interface? friend with wrapper?)
		Result runMessageReceiver();
		
		Result sendMessage(const std::string &message);
		
		// Network cache stuff
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
	
		State m_state;
		//pthread_mutex_t m_stateMutex;
		threading::Mutex m_stateMutex;
		
		TCPConnection m_connection;
		
		threading::Thread m_receiverThread;
		std::auto_ptr<MessageSender> m_messageSender;
		
		PluginManager *m_pluginManager;
		
		irc::NetworkCache m_networkCache;
	};
} // namespace firc
} // namespace anp

#endif // _NETWORKMANAGER_H_
