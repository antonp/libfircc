#include <basedefs.h>
#include <vector>
#include <string>
#include <networkcache.h>
#include <channelcache.h>
#include <userinfo.h>
#include <stdexcept>

namespace anp
{
namespace firc
{
	
	// pImpl object
	class NetworkCacheImpl
	{
	public:
		const ChannelCache *channel(const std::string &name) const;
		ChannelCache *channel(const std::string &name);
		const ChannelCache *getChannelCopy(
										const std::string &name) const;
		
		UserInfo *userByName(const std::string &name);
	private:
		uint32 getChannelIndex(const std::string &name) const;
		uint32 getUserIndex(const std::string &name) const;
		std::vector<ChannelCache *> m_channels;
		std::vector<UserInfo *> m_users;
	};

	const ChannelCache *NetworkCacheImpl::channel(
										const std::string &name) const
	{
		uint32 index = getChannelIndex(name);
		
		return m_channels[index];
	}

	ChannelCache *NetworkCacheImpl::channel(
										const std::string &name)
	{
		uint32 index = getChannelIndex(name);
		
		return m_channels[index];
	}
	
	
	const ChannelCache *NetworkCacheImpl::getChannelCopy(
										const std::string &name) const
	{
		const ChannelCache *const srcChannel = this->channel(name);
		return new ChannelCache(*srcChannel);
	}

	UserInfo *NetworkCacheImpl::userByName(const std::string &name) {
		// ..
	}
	
	uint32 NetworkCacheImpl::getChannelIndex(
										const std::string &name) const
	{
		// Perform a binary search
		uint32 upper = m_channels.size();
		uint32 lower = 0;
		uint32 pivot = 0;
		uint32 diff = 0;
		while ( true )
		{
			pivot = (upper-lower)/2;
			diff=name.compare(m_channels[pivot]->name());
			if ( diff < 0 )
			{
				upper = pivot;
			} else if ( diff > 0 )
			{
				lower = pivot;
			} else // diff = 0
			{
				// Match
				return pivot;
			}
			// TODO: If (upper-lower) is small, what happens?
			// Will all positions be examined?
			// Do we need to roundup pivot calculation?
		}
		throw std::runtime_error("Unable to find channel.");
	}

	uint32 NetworkCacheImpl::getUserIndex(
										const std::string &name) const
	{
		// Perform a binary search
		uint32 upper = m_users.size();
		uint32 lower = 0;
		uint32 pivot = 0;
		uint32 diff = 0;
		while ( true )
		{
			pivot = (upper-lower)/2;
			diff=name.compare(m_users[pivot]->name());
			if ( diff < 0 )
			{
				upper = pivot;
			} else if ( diff > 0 )
			{
				lower = pivot;
			} else // diff = 0
			{
				// Match
				return pivot;
			}
			// TODO: If (upper-lower) is small, what happens?
			// Will all positions be examined?
			// Do we need to roundup pivot calculation?
		}
		throw std::runtime_error("Unable to find channel.");
	}

	// Wrapper functions
	
	const ChannelCache *NetworkCache::getChannelCopy(
										const std::string &name) const
	{
		return m_impl->getChannelCopy(name);
	}
	
	void NetworkCache::addUserToChannel(const std::string &name,
							  const std::string &user,
							  const std::string &host,
							  const std::string &channelName)
	{
		ChannelCache *channel = m_impl->channel(channelName);
		const UserInfo *userInfo = NULL;
		try
		{
			userInfo = m_impl->userByName(name);
		} catch ( std::runtime_error &e )
		{
			userInfo = new UserInfo(name, user, host);
		}
		channel->addUser(userInfo);
		userInfo->addChannel(channel);
	}
}
}
