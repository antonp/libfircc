#include <basedefs.h>
#include <list>
#include <string>
#include <channelcache.h>

namespace anp
{
namespace firc
{
	class ChannelCache;
	
	// pImpl object
	class NetworkCacheImpl
	{
	public:
		const ChannelCache *channel(const std::string &name) const;
	private:
		uint32 getTableIndex(const std::string &name) const;
		enum CONSTANTS
		{
			HASHTABLE_SIZE=40
		};
		std::list<ChannelCache *> m_channelHashTable[HASHTABLE_SIZE];
	};
	
	const ChannelCache *NetworkCacheImpl::channel(const std::string &name) const
	{
		uint32 index = getTableIndex(name);
		const std::list<ChannelCache *> &list = m_channelHashTable[index];
		
		std::list<ChannelCache *>::const_iterator i;
		for ( i=list.begin(); i != list.end(); i++ )
		{
			if ( (*i)->name() == name )
			{
				return (*i);
			}
		}
		
		return NULL;
	}
	
	uint32 NetworkCacheImpl::getTableIndex(const std::string &name) const
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
	
	// Wrapper functions
}
}
