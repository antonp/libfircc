#include <basedefs.h>
#include <string>
#include "../channelcache.h"

namespace anp
{
namespace firc
{
	class ChannelCacheImpl
	{
	public:
		ChannelCacheImpl(const std::string &name);
		ChannelCacheImpl(const ChannelCacheImpl &impl);
		const std::string &name() const;
		
	private:
		std::string m_name;
	};
	
	ChannelCacheImpl::ChannelCacheImpl(const std::string &name):
	m_name(name)
	{
		// ...
	}
	
	ChannelCacheImpl::ChannelCacheImpl(const ChannelCacheImpl &impl):
	m_name(impl.m_name)
	{
		// ...
	}
	
	const std::string &ChannelCacheImpl::name() const
	{
		return m_name;
	}
	
	
	
	ChannelCache::ChannelCache(const std::string &name):
	m_impl(new ChannelCacheImpl(name))
	{
		// ...
	}
	
	ChannelCache::ChannelCache(const ChannelCache &channelCache):
	m_impl(new ChannelCacheImpl(*channelCache.m_impl))
	{
		
	}
	
	const std::string &ChannelCache::name() const
	{
		return m_impl->name();
	}
}
}
