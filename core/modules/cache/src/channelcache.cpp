#include <basedefs.h>
#include <string>
#include "../channelcache.h"
#include "../userinfo.h"
#include <list>
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
		const UserInfo *userInfo(const std::string &name) const;
	private:
		uint32 getTableIndex(const std::string &name) const;
		enum CONSTANTS
		{
			HASHTABLE_SIZE = 40
		};
		std::list<UserInfo *> m_userHashTable[HASHTABLE_SIZE];
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
	
	const UserInfo *ChannelCacheImpl::userInfo(
										const std::string &name) const
	{
		uint32 index = this->getTableIndex(name);
		const std::list<UserInfo *> &list =
											m_userHashTable[index];
		
		std::list<UserInfo *>::const_iterator i;
		for ( i=list.begin(); i != list.end(); i++ )
		{
			if ( (*i)->name() == name )
			{
				return (*i);
			}
		}
		
		throw std::runtime_error("Unable to find channel.");
		return NULL;
	}

	uint32 ChannelCacheImpl::getTableIndex(
										const std::string &name) const
	{
		uint32 length = name.length();
		uint32 index = 0;
		for ( uint32 i=0; i<length; ++i )
		{
			index = name[i]*2 - name[i];
		}
		index = index % HASHTABLE_SIZE;
		return index;
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
