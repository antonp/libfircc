#ifndef _NETWORKCACHE_PUBLIC_H_
#define _NETWORKCACHE_PUBLIC_H_

namespace anp
{
namespace firc
{
	class ChannelCache;
	
	class INetworkCachePublic
	{
	public:
		virtual const ChannelCache *getChannelCopy(
									const std::string &name) const = 0;
	};
}
}

#endif // _NETWORKCACHE_PUBLIC_H_
