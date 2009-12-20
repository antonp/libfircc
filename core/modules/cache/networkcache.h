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
	private:
		NetworkCacheImpl *m_impl;
	};
}	
}

#endif // _NETWORKCACHE_H_
