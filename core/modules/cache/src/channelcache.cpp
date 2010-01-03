#include <basedefs.h>
#include <string>
#include "../channelcache.h"
#include "../userinfo.h"
#include "../inc/utilities.h"
#include <vector>
#include <algorithm>
#include <stdexcept>

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
		
		std::string m_topic;
	private:
		std::string m_name;
	};
	
	ChannelCacheImpl::ChannelCacheImpl(const std::string &name):
	m_name(name)
	{
		// ...
	}
	
	ChannelCacheImpl::ChannelCacheImpl(const ChannelCacheImpl &impl):
	m_name(impl.m_name),
	m_topic(impl.m_topic)
	{
		// ...
	}
	
	const std::string &ChannelCacheImpl::name() const
	{
		return m_name;
	}
	
	// Wrappers/publics

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

	void ChannelCache::setTopic(const std::string &topic)
	{
		m_impl->m_topic = topic;
	}

	const std::string &ChannelCache::topic() const
	{
		return m_impl->m_topic;
	}
}
}
