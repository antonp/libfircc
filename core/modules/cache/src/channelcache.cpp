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
		ChannelCacheImpl(const std::string &name,
						const std::string &topic);
		ChannelCacheImpl(const ChannelCacheImpl &impl);

		const std::string &name() const;
		
		std::string m_topic;
	private:
		std::string m_name;
	};
	
	ChannelCacheImpl::ChannelCacheImpl(const std::string &name,
										const std::string &topic):
	m_name(name),
	m_topic(topic)
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

	ChannelCache::ChannelCache(const std::string &name,
								const std::string &topic):
	m_impl(new ChannelCacheImpl(name, topic))
	{

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
