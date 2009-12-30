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
		
		const ChannelCache *getChannelCopy(
									const std::string &name) const;
									
		void addUserToChannel(const std::string &name,
							  const std::string &user,
							  const std::string &host,
							  const std::string &channelName);
		void removeUserFromChannel(const std::string &name,
									const std::string &channelName);
		void setTopic(const std::string &channelName,
						const std::string &topic);
	private:
		NetworkCacheImpl *m_impl;
	};
}	
}

#endif // _NETWORKCACHE_H_
