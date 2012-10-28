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

#ifndef _NETWORKCACHE_USERINTERFACE_H_
#define _NETWORKCACHE_USERINTERFACE_H_

#include <string>
#include <anpcode/iwritablecontainer.h>

namespace anp
{
namespace irc
{
    class ChannelCache;

    /**
     * This interface allows for querying the cache for information stored
     * about the network.
     */
    class NetworkCacheUserInterface
    {
    public:
        virtual ~NetworkCacheUserInterface() { }

        /**
        Copy information about a particular channel into an
        already allocated ChannelCache object.

        @param name
        The name of the channel.

        @param dest
        The channel information will be written to this ChannelCache
        object.
        */
        virtual void getChannel(const std::string &name,
                                        ChannelCache &dest) const = 0;

        /**
        Copies the client nick name into the clientNickName
        parameter.

        @param clientNickName
        The client nickname will be written to this string.
        */
        virtual void getClientNickName(std::string &clientNickName)
                                                            const = 0;

        /**
        Represents a user in a specific channel.
        */
        struct UserInChannel
        {
            UserInChannel() { }
            UserInChannel(const std::string &nick_in,
                          const std::string &modes_in):
                          nick(nick_in), modes(modes_in) { }
            std::string nick;
            std::string modes;
        };

        /**
         * Retrieves the userlist of a channel.
         *
         * @param name
         * The name of the channel.
         *
         * @param userList
         * This container will have the current userlist written to it.
         */
        virtual void getUsersInChannel(const std::string &name,
            anp::IWritableContainer<UserInChannel> &userList) const = 0;

        /**
        Retrieves a single user from a channel.

        @param channel
        Name of the channel.

        @param nick
        Nick of the user.

        @param dest
        UserInChannel instance to which the user info will be written.
        */
        virtual void getUserInChannel(const std::string &channel,
                                      const std::string &nick,
                                      UserInChannel &dest) const = 0;
    };
}
}

#endif // _NETWORKCACHE_USERINTERFACE_H_
