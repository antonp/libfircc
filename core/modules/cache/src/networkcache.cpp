/*

Copyright (c) 2010, Anton Petersson < anton (at) anp.nu >
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <basedefs.h>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <networkcache.h>
#include <channelcache.h>
#include <userinfo.h>
#include "../inc/utilities.h"
#include <stdexcept>
#include <log_singleton.h>

namespace anp
{
namespace firc
{

	struct ChannelUserRelation
	{
		ChannelUserRelation(const std::string &channel,
									const std::string &user, uint32 modes):
		m_channel(channel),
		m_user(user),
		m_modes(modes)
		{
		}
		uint32 m_modes;
		std::string m_channel;
		std::string m_user;
	};

	static int channeluserrelation_compareC(const ChannelUserRelation &r1,
									const ChannelUserRelation &r2)
	{
		// TODO: Improve this by also taking nick names into account
		// So that the table can be sorted on channel (primarily) and
		// nick names (secondarily)
		return r1.m_channel.compare(r2.m_channel);
	}

	static int channeluserrelation_compareCU(const ChannelUserRelation &r1,
									const ChannelUserRelation &r2)
	{
		int ret = r1.m_channel.compare(r2.m_channel);
		if ( 0 == ret )
		{
			return r1.m_user.compare(r2.m_user);
		}
		return ret;
	}

	// pImpl object
	class NetworkCacheImpl
	{
	public:
		const ChannelCache *channel(const std::string &name) const;
		ChannelCache *channel(const std::string &name);
		const ChannelCache *getChannelCopy(
										const std::string &name) const;
		void getChannelCopy(const std::string &name,
												ChannelCache &dest) const;
		
		UserInfo *userByName(const std::string &name);
		std::vector<ChannelCache *> m_channels;
		std::vector<ChannelUserRelation> m_cuRelations;
		std::string m_clientNickName;
		LogSingletonHelper m_log;
	private:
		uint32 getChannelIndex(const std::string &name) const;
		uint32 getUserIndex(const std::string &name) const;
		std::vector<UserInfo *> m_users;
	};

	const ChannelCache *NetworkCacheImpl::channel(
										const std::string &name) const
	{
//		uint32 index = getChannelIndex(name);
		ChannelCache tempChan(name);
		std::vector<ChannelCache *>::const_iterator i = std::lower_bound(
			m_channels.begin(), m_channels.end(), &tempChan,

		channelinfo_compare);
		if ( i != m_channels.end() )
		{
			return (*i);
		} else
		{
			throw std::runtime_error("Unable to find channel.");
		}
		throw std::logic_error("Reached an unreachable point in code.");
		return NULL; // Should never happen
	}

	ChannelCache *NetworkCacheImpl::channel(
										const std::string &name)
	{
		ChannelCache tempChan(name);
		std::vector<ChannelCache *>::iterator i = std::lower_bound(
			m_channels.begin(), m_channels.end(), &tempChan,

		channelinfo_compare);
		if ( i != m_channels.end() )
		{
			return (*i);
		} else
		{
			throw std::runtime_error("Unable to find channel.");
		}
		throw std::logic_error("Reached an unreachable point in code.");
		return NULL; // Should never happen
	}

	const ChannelCache *NetworkCacheImpl::getChannelCopy(
										const std::string &name) const
	{
		// TODO: Maybe take a ChannelCache ref to avoid memalloc?
		const ChannelCache *const srcChannel = this->channel(name);
		return new ChannelCache(*srcChannel);
	}

	void NetworkCacheImpl::getChannelCopy(const std::string &name,
									ChannelCache &dest) const
	{
		dest = *this->channel(name);
	}

	UserInfo *NetworkCacheImpl::userByName(const std::string &name) {
		UserInfo temp(name, "", "");
		std::pair<
			std::vector<UserInfo *>::iterator,
			std::vector<UserInfo *>::iterator
		> res = std::equal_range(m_users.begin(), m_users.end(),
							&temp, userinfo_compare);
		if ( res.first != m_users.end() )
		{
			return *res.first;
		} else
		{
			throw std::runtime_error("Unable to find user.");
			return NULL;
		}
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
	NetworkCache::NetworkCache():
	m_impl(new NetworkCacheImpl)
	{
	}

	NetworkCache::~NetworkCache()
	{
		delete m_impl;
	}
	
	const ChannelCache *NetworkCache::getChannel(
										const std::string &name) const
	{
		return m_impl->getChannelCopy(name);
	}

	void NetworkCache::getChannel(const std::string &name,
									ChannelCache &dest) const
	{
		m_impl->getChannelCopy(name, dest);
	}

	void NetworkCache::addChannel(const std::string &channel)
	{
		// TODO: Cache this channel here, since "addUserToChannel" will
		// likely be called next, upon some RPL_NAMREPLY
		ChannelCache *temp = new ChannelCache(channel);
		std::vector<ChannelCache *> &list = m_impl->m_channels;
		list.insert(std::lower_bound(list.begin(),
			list.end(), temp, channelinfo_compare), temp);
	}

	void NetworkCache::removeChannel(const std::string &channel)
	{
		ChannelCache temp(channel);
		std::vector<ChannelCache *> &list = m_impl->m_channels;
		std::vector<ChannelCache *>::iterator i = 
		i = std::lower_bound(list.begin(),
			list.end(), &temp, channelinfo_compare);
		if ( i != list.end() )
		{
			delete (*i);
			list.erase(i);
		}

		// Remove entries in relation table
		removeAllUsersFromChannel(channel);
	}

	void NetworkCache::addUserToChannel(const std::string &name,
							  const std::string &user,
							  const std::string &host,
							  const std::string &channelName)
	{
		ChannelCache *channel = m_impl->channel(channelName);
		UserInfo *userInfo = NULL;
		try
		{
			userInfo = m_impl->userByName(name);
		} catch ( std::runtime_error &e )
		{
			userInfo = new UserInfo(name, user, host);
		}
		ChannelUserRelation newRelation(channelName, name, 0);
		std::vector<ChannelUserRelation> &table = m_impl->m_cuRelations;

		// Avoid duplicates (drop duplicates silently without
		// exceptions or anything)
		if ( table.empty() || !std::binary_search(table.begin(), 
				table.end(), newRelation,
				channeluserrelation_compareCU) )
		{
			std::stringstream ss;
			ss << "(cache) Added '" << name << "' to '"
				<< channelName << "'.";
			m_impl->m_log(ss.str());
			table.insert(
				std::lower_bound(
					table.begin(),
					table.end(),
					newRelation,
					channeluserrelation_compareCU
				),
				newRelation
			);
		} else
		{
			std::stringstream ss;
			ss << "(cache) Didn't add '" << name << "' to '"
				<< channelName << "' because of binary_search. table.size()="
				<< table.size();
			m_impl->m_log(ss.str());
		}
	}
	
	/**
		Removes a channel - user relation.

		@param name
		Nickname of the user.

		@param channelName
		Name of the channel.

		@remark
		Assumes there's no duplicate entries.
	*/
	void NetworkCache::removeUserFromChannel(const std::string &name,
								const std::string &channelName)
	{
		ChannelUserRelation tempRel(channelName, name, 0);
		std::vector<ChannelUserRelation> &table = m_impl->m_cuRelations;
		std::pair<
			std::vector<ChannelUserRelation>::iterator,
			std::vector<ChannelUserRelation>::iterator
		> range = std::equal_range(table.begin(), table.end(), tempRel,
									channeluserrelation_compareC);

		if ( range.first != table.end() ) // equal_range() succeeded?
		{
			// infinite loop while ( range.first != range.second )
			bool32 erased = false;
			while ( !erased && range.first != range.second )
			{
				if ( (*range.first).m_user == name )
				{
					range.first = table.erase(range.first);
					std::stringstream ss;
					ss << "(cache) Removed '" << name
						<< "' from '" << channelName << "'.";
					m_impl->m_log(ss.str());
					erased = true;
				} else
				{
					range.first++;
				}
			}
		} else
		{
			std::stringstream ss;
			ss << "Unable to find channel '" << channelName << "'.";
			m_impl->m_log(ss.str());
			throw std::runtime_error(ss.str());
		}
	}

	void NetworkCache::removeAllUsersFromChannel(
								const std::string &channel)
	{
		ChannelUserRelation tempRel(channel, "", 0);
		std::vector<ChannelUserRelation> &table = m_impl->m_cuRelations;
		std::pair<
			std::vector<ChannelUserRelation>::iterator,
			std::vector<ChannelUserRelation>::iterator
		> range = std::equal_range(table.begin(), table.end(), tempRel,
									channeluserrelation_compareC);
		
		std::stringstream ss;
		ss << "Erasing all users in " << channel;
		m_impl->m_log(ss.str());
		table.erase(range.first, range.second);
	}

	void NetworkCache::setTopic(const std::string &channelName,
								const std::string &topic)
	{
		ChannelCache *channel = m_impl->channel(channelName);
		channel->setTopic(topic);
	}

	void NetworkCache::setClientNickName(
							const std::string &clientNickName)
	{
		m_impl->m_clientNickName = clientNickName;
	}

	void NetworkCache::getClientNickName(
							std::string &clientNickName) const
	{
		clientNickName = m_impl->m_clientNickName;
	}
}
}
