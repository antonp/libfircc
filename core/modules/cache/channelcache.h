#ifndef _CHANNELCACHE_H_
#define _CHANNELCACHE_H_

#include <string>

namespace anp
{
namespace firc
{
	class ChannelCacheImpl;
	
	class ChannelCache
	{
	public:
		ChannelCache(const std::string &name="",
						const std::string &topic="");
		ChannelCache(const ChannelCache &channelCache);
	
		const std::string &name() const;
		void setTopic(const std::string &topic);
		const std::string &topic() const;
	private:
		ChannelCacheImpl *m_impl;
	};
}
}

#endif // _CHANNELCACHE_H_
