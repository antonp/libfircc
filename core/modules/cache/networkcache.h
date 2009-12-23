#ifndef _NETWORKCACHE_H_
#define _NETWORKCACHE_H_

#include "networkcache_public.h"

namespace anp
{
namespace firc
{
	class NetworkCacheImpl;
	
	class NetworkCache: public INetworkCachePublic
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
		
	private:
		NetworkCacheImpl *m_impl;
	};
}	
}

#endif // _NETWORKCACHE_H_
