#ifndef _CHANNELCACHE_H_
#define _CHANNELCACHE_H_

namespace anp
{
namespace firc
{
	class ChannelCacheImpl;
	
	class ChannelCache
	{
	public:
		ChannelCache(const std::string &name);
		ChannelCache(const ChannelCache &channelCache);
	
		const std::string &name() const;
	private:
		ChannelCacheImpl *m_impl;
	};
}
}

#endif // _CHANNELCACHE_H_
