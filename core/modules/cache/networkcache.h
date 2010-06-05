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

#ifndef _NETWORKCACHE_H_
#define _NETWORKCACHE_H_

#include "networkcache_userinterface.h"

namespace anp
{
namespace firc
{
	class NetworkCacheImpl;
	
	class NetworkCache: public NetworkCacheUserInterface
	{
	public:
		NetworkCache();
		~NetworkCache();
		
		void getChannel(const std::string &name,
										ChannelCache &dest) const;

		void addChannel(const std::string &channel);
		void removeChannel(const std::string &channel);

		void addUserToChannel(const std::string &name,
							  const std::string &user,
							  const std::string &host,
							  const std::string &channelName);
		void removeUserFromChannel(const std::string &name,
									const std::string &channelName);
		void removeAllUsersFromChannel(
							const std::string &channel);

		void setTopic(const std::string &channelName,
						const std::string &topic);
		void setClientNickName(const std::string &clientNickName);
		void getClientNickName(std::string &clientNickName) const;
	private:
		NetworkCacheImpl *m_impl;
	};
}	
}

#endif // _NETWORKCACHE_H_
