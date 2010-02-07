#ifndef _NETWORKCACHE_H_
#define _NETWORKCACHE_H_

#include "networkcache_userinterface.h"

namespace anp
{
namespace firc
{
	class NetworkCacheImpl;
	
	class NetworkCache: public NetworkCacheUserInterface
	{
	public:
		NetworkCache();
		~NetworkCache();
		
		const ChannelCache *getChannel(
									const std::string &name) const;
		void getChannel(const std::string &name,
										ChannelCache &dest) const;

		void addChannel(const std::string &channel);
		void removeChannel(const std::string &channel);

		void addUserToChannel(const std::string &name,
							  const std::string &user,
							  const std::string &host,
							  const std::string &channelName);
		void removeUserFromChannel(const std::string &name,
									const std::string &channelName);
		void removeAllUsersFromChannel(
							const std::string &channel);

		void setTopic(const std::string &channelName,
						const std::string &topic);
		void setClientNickName(const std::string &clientNickName);
		void getClientNickName(std::string &clientNickName) const;
	private:
		NetworkCacheImpl *m_impl;
	};
}	
}

#endif // _NETWORKCACHE_H_
