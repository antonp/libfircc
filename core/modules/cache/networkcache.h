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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef _NETWORKCACHE_H_
#define _NETWORKCACHE_H_

#include "networkcache_userinterface.h"
#include <networkevents.h>
#include <networkeventsubscribers.h>

namespace anp
{
namespace irc
{
	class NetworkCacheImpl;

	/**
	 * This class stores information about the state of a particular network.
	 * 
	 * It tracks channels and users.
	 */
	class NetworkCache: public NetworkCacheUserInterface,
						public anp::irc::eventsubscribers::NumericReply,
						public anp::irc::eventsubscribers::Join,
						public anp::irc::eventsubscribers::Part,
						public anp::irc::eventsubscribers::Topic
	{
	public:
		NetworkCache();
		~NetworkCache();
		
		/**
		 * Retrieves a snapshot of a channel.
		 * 
		 * @param name
		 * Name of the channel.
		 * 
		 * @param dest
		 * Destination of the snapshot.
		 */
		void getChannel(const std::string &name,
										ChannelCache &dest) const;

		/**
		 * Adds a channel to the cache.
		 * 
		 * @param channel
		 * Name of the channel.
		 */
		void addChannel(const std::string &channel);
		
		/**
		 * Removes all information known about a channel.
		 * 
		 * @param channel
		 * Name of the channel to remove.
		 */
		void removeChannel(const std::string &channel);

		/**
		 * Adds a user to a channel.
		 * 
		 * @param name
		 * Nick name of user.
		 * 
		 * @param user
		 * User name of user.
		 * 
		 * @param host
		 * Host name of user.
		 * 
		 * @param channelName
		 * Name of channel.
		 */
		void addUserToChannel(const std::string &name,
							  const std::string &user,
							  const std::string &host,
							  const std::string &channelName);
							  
		/**
		 * Removes a user from a channel.
		 * 
		 * @param name
		 * Nick name of the user.
		 * 
		 * @param channelName
		 * Name of the channel.
		 */
		void removeUserFromChannel(const std::string &name,
									const std::string &channelName);
									
		/**
		 * Removes all users from a channel.
		 * 
		 * @param channel
		 * Name of channel.
		 */
		void removeAllUsersFromChannel(
							const std::string &channel);

		/**
		 * Sets the topic of a channel.
		 * 
		 * @param channelName
		 * Name of channel.
		 * 
		 * @param topic
		 * New topic.
		 */
		void setTopic(const std::string &channelName,
						const std::string &topic);
						
		/**
		 * Sets the client nick name.
		 * 
		 * @param clientNickName
		 * Client nick name.
		 */
		void setClientNickName(const std::string &clientNickName);
		
		/**
		 * Retrieves the client nick name.
		 * 
		 * @param [out] clientNickName
		 * Destination of nick name.
		 */
		void getClientNickName(std::string &clientNickName) const;
	private:
		void receiveEvent(anp::irc::events::NumericReply &event);
		void receiveEvent(anp::irc::events::Join &event);
		void receiveEvent(anp::irc::events::Part &event);
		void receiveEvent(anp::irc::events::Topic &event);

		NetworkCacheImpl *m_impl;
	};
}	
}

#endif // _NETWORKCACHE_H_
