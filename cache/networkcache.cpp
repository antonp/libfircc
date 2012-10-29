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

#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <fircc/networkcache.h>
#include <fircc/channelcache.h>
#include "inc/userinfo.h"
#include "inc/utilities.h"
#include "../network/inc/tokenizer.h" // TODO move tokenizer.h to /inc/
#include "inc/modeconv.h"
#include <stdexcept>
#include <anpcode/log_singleton.h>
#include <anpcode/anp_threading.h>
#include <stdint.h>

namespace anp
{
namespace irc
{

    struct ChannelUserRelation
    {
        ChannelUserRelation(const std::string &channel,
                            const std::string &user,
                            const std::string &modes):
        m_channel(channel),
        m_user(user),
        m_modes(modes)
        {
        }
        std::string m_channel;
        std::string m_user;
        std::string m_modes;
    };

    // Use to sort on channel only
    static int channeluserrelation_compareC(const ChannelUserRelation &r1,
                                            const ChannelUserRelation &r2)
    {
        return r1.m_channel.compare(r2.m_channel);
    }

    // Use to sort on channel, user
    static int channeluserrelation_compareCU(const ChannelUserRelation &r1,
                                             const ChannelUserRelation &r2)
    {
        int ret = r1.m_channel.compare(r2.m_channel);
        if ( 0 == ret )
        {
            //return r1.m_user.compare(r2.m_user);
            return (r1.m_user < r2.m_user);
        }
        return ret;
    }

    // pImpl object
    class NetworkCacheImpl
    {
    public:
        ~NetworkCacheImpl();

        const ChannelCache *channel(const std::string &name) const;
        ChannelCache *channel(const std::string &name);
        const ChannelCache *getChannelCopy(
                                        const std::string &name) const;
        void getChannelCopy(const std::string &name,
                                                ChannelCache &dest) const;
        void removeUserFromAllChannels(const std::string &nick);
        void updateNick(const std::string &nick, const std::string &newNick);
        /**
        Lookup channel user relation entry.

        @remark
        Note that this method doesn't lock.. 
        */
        ChannelUserRelation &cuRel(const std::string &channel,
                   const std::string &user);

        UserInfo *userByName(const std::string &name);
        std::vector<ChannelCache *> m_channels;
        std::vector<ChannelUserRelation> m_cuRelations;
        std::string m_clientNickName;
        LogSingletonHelper m_log;
        anp::threading::Mutex m_mutex;
    private:
        std::vector<UserInfo *> m_users;
    };

    NetworkCacheImpl::~NetworkCacheImpl()
    {
        anp::threading::Lock lock(m_mutex);

        std::vector<ChannelCache *>::iterator c;
        for ( c=m_channels.begin(); c != m_channels.end(); c++ )
        {
            delete (*c);
        }
        m_channels.clear();

        std::vector<UserInfo *>::iterator u;
        for ( u=m_users.begin(); u != m_users.end(); u++ )
        {
            delete (*u);
        }
        m_users.clear();
    }

    const ChannelCache *NetworkCacheImpl::channel(
                                        const std::string &name) const
    {
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
            std::stringstream err;
            err << "Unable to find channel: " << name;
           throw std::runtime_error(err.str());
        }
        throw std::logic_error("Reached an unreachable point in code.");
        return NULL; // Should never happen
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
            return NULL;
        }
    }
    
    void NetworkCacheImpl::removeUserFromAllChannels(const std::string &nick)
    {
        // Somewhat naive implementation for now
        for ( std::vector<ChannelUserRelation>::iterator i
              =m_cuRelations.begin();
              i != m_cuRelations.end();
              i++ )
        {
            if ( (*i).m_user == nick )
            {
                i = m_cuRelations.erase(i);
            }
        }
    }

    void NetworkCacheImpl::updateNick(const std::string &nick,
                                      const std::string &newNick)
    {
        for ( std::vector<ChannelUserRelation>::iterator i
              =m_cuRelations.begin();
              i != m_cuRelations.end();
              i++ )
        {
            if ( (*i).m_user == nick )
            {
                (*i).m_user = newNick;
            }
        }
    }

    ChannelUserRelation &NetworkCacheImpl::cuRel(const std::string &channel,
                                                 const std::string &user)
    {
        ChannelUserRelation tempRel(channel, user, "");
        std::pair<
            std::vector<ChannelUserRelation>::iterator,
            std::vector<ChannelUserRelation>::iterator
        > range = std::equal_range(m_cuRelations.begin(), m_cuRelations.end(),
                                   tempRel, channeluserrelation_compareCU);

        if ( range.first != m_cuRelations.end() )
        {
            return *range.first;
        } else
        {
            // Generate some debug output: all cu relations
            std::stringstream ts;
            ts << "Dump cu relations: ";
            for ( unsigned int i=0; i<m_cuRelations.size(); i++ )
            {
                ts << "'" << m_cuRelations[i].m_channel << "':'" <<
                    m_cuRelations[i].m_user << "', ";
            }
            ANPLOGD("libfircc-cache", ts.str());

            std::stringstream ss;
            ss << "Couldn't find channel-user relation in cache: "
                << channel << ", '" << user << '\'';
            throw std::runtime_error(ss.str());
        }
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

    void NetworkCache::getChannel(const std::string &name,
                                    ChannelCache &dest) const
    {
        anp::threading::Lock lock(m_impl->m_mutex);

        m_impl->getChannelCopy(name, dest);
    }

    void NetworkCache::addChannel(const std::string &channel)
    {
        anp::threading::Lock lock(m_impl->m_mutex);

        // TODO: Cache this channel here, since "addUserToChannel" will
        // likely be called next, upon some RPL_NAMREPLY
        ChannelCache *temp = new ChannelCache(channel);
        std::vector<ChannelCache *> &list = m_impl->m_channels;
        list.insert(std::lower_bound(list.begin(),
            list.end(), temp, channelinfo_compare), temp);
    }

    void NetworkCache::removeChannel(const std::string &channel)
    {
        {
            anp::threading::Lock lock(m_impl->m_mutex);

            ChannelCache temp(channel);
            std::vector<ChannelCache *> &list = m_impl->m_channels;
            std::vector<ChannelCache *>::iterator i =
                std::lower_bound(list.begin(),
                list.end(), &temp, channelinfo_compare);
            if ( i != list.end() )
            {
                delete (*i);
                list.erase(i);
            }
        } // End of lock scope.
        // Remove entries in relation table
        removeAllUsersFromChannel(channel);
    }

    void NetworkCache::addUserToChannel(const std::string &name,
                                        const std::string &user,
                                        const std::string &host,
                                        const std::string &channelName,
                                        char mode)
    {
        anp::threading::Lock lock(m_impl->m_mutex);

        // Verify that this is a known channel
        // else exception
        m_impl->channel(channelName);

        ChannelUserRelation newRelation(channelName,
                                        name,
                                        mode == ' ' ? "" : std::string(1, mode));
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
            ANPLOGD("libfirc", ss.str());
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
            ANPLOGD("libfirc", ss.str());
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
        anp::threading::Lock lock(m_impl->m_mutex);

        ChannelUserRelation tempRel(channelName, name, "");
        std::vector<ChannelUserRelation> &table = m_impl->m_cuRelations;
        std::pair<
            std::vector<ChannelUserRelation>::iterator,
            std::vector<ChannelUserRelation>::iterator
        > range = std::equal_range(table.begin(), table.end(), tempRel,
                                    channeluserrelation_compareC);

        if ( range.first != table.end() ) // equal_range() succeeded?
        {
            // infinite loop while ( range.first != range.second )
            bool erased = false;
            while ( !erased && range.first != range.second )
            {
                if ( (*range.first).m_user == name )
                {
                    range.first = table.erase(range.first);
                    std::stringstream ss;
                    ss << "(cache) Removed '" << name
                        << "' from '" << channelName << "'.";
                    ANPLOGD("libfirc", ss.str());
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
            ANPLOGD("libfirc", ss.str());
            throw std::runtime_error(ss.str());
        }
    }

    void NetworkCache::removeAllUsersFromChannel(
                                const std::string &channel)
    {
        anp::threading::Lock lock(m_impl->m_mutex);

        ChannelUserRelation tempRel(channel, "", "");
        std::vector<ChannelUserRelation> &table = m_impl->m_cuRelations;
        std::pair<
            std::vector<ChannelUserRelation>::iterator,
            std::vector<ChannelUserRelation>::iterator
        > range = std::equal_range(table.begin(), table.end(), tempRel,
                                    channeluserrelation_compareC);

        std::stringstream ss;
        ss << "Erasing all users in " << channel;
        ANPLOGD("libfirc", ss.str());
        table.erase(range.first, range.second);
    }

    void NetworkCache::setTopic(const std::string &channelName,
                                const std::string &topic)
    {
        anp::threading::Lock lock(m_impl->m_mutex);

        ChannelCache *channel = m_impl->channel(channelName);
        channel->setTopic(topic);
    }

    void NetworkCache::setClientNickName(
                            const std::string &clientNickName)
    {
        anp::threading::Lock lock(m_impl->m_mutex);

        m_impl->m_clientNickName = clientNickName;
    }

    void NetworkCache::getClientNickName(
                            std::string &clientNickName) const
    {
        anp::threading::Lock lock(m_impl->m_mutex);

        clientNickName = m_impl->m_clientNickName;
    }

    void NetworkCache::getUsersInChannel(const std::string &name,
                        anp::IWritableContainer<UserInChannel> &userList) const
    {
        anp::threading::Lock lock(m_impl->m_mutex);

        ChannelUserRelation tempRel(name, "", "");
        std::vector<ChannelUserRelation> &table = m_impl->m_cuRelations;
        std::pair<
            std::vector<ChannelUserRelation>::iterator,
            std::vector<ChannelUserRelation>::iterator
        > range = std::equal_range(table.begin(), table.end(), tempRel,
                                    channeluserrelation_compareC);

        if ( range.first != table.end() ) // equal_range() succeeded?
        {
            while ( range.first != range.second )
            {
                userList.pushBack(UserInChannel((*range.first).m_user,
                                                (*range.first).m_modes));
                range.first++;
            }
        } else
        {
            std::stringstream ss;
            ss << "Unable to find channel '" << name << "'.";
            ANPLOGD("libfirc", ss.str());
            throw std::runtime_error(ss.str());
        }
    }

    void NetworkCache::getUserInChannel(const std::string &channel,
                                        const std::string &nick,
                                        UserInChannel &dest) const
    {
        anp::threading::Lock lock(m_impl->m_mutex);

        ChannelUserRelation tempRel(channel, nick, "");
        std::vector<ChannelUserRelation> &table = m_impl->m_cuRelations;
        std::pair<
            std::vector<ChannelUserRelation>::iterator,
            std::vector<ChannelUserRelation>::iterator
        > range = std::equal_range(table.begin(), table.end(), tempRel,
                                    channeluserrelation_compareCU);

        if ( range.first != table.end() )
        {
            dest.nick = (*range.first).m_user;
            dest.modes = (*range.first).m_modes;
        } else
        {
            std::stringstream ss;
            ss << "Unable to find user '" << nick << "' in channel '"
                << channel << "'.";
            ANPLOGD("libfircc", ss.str());
            throw std::runtime_error(ss.str());
        }
    }

    /////////////////////////////////
    // Event subscriber interfaces

    void NetworkCache::receiveEvent(anp::irc::events::NumericReply &event)
    {
        const std::string &command = event.command();

        if ( command == "332" ) // RPL_TOPIC
        {
            setTopic(event.param(1), event.param(2));
        } else if ( command == "353" ) // RPL_NAMREPLY
        {
           handle_RPL_NAMREPLY(event.param(2), event.param(3)); 
        }
    }

    void NetworkCache::receiveEvent(anp::irc::events::Join &event)
    {
        const MsgPrefix &origin = event.origin();
        std::string clientNickName;

        getClientNickName(clientNickName);
        if ( origin.nick() == clientNickName )
        {
            addChannel(event.channel());
        }
        addUserToChannel(origin.nick(),
                         origin.user(),
                         origin.host(),
                         event.channel(),
                         ' ');
    }

    void NetworkCache::receiveEvent(anp::irc::events::Part &event)
    {
        const MsgPrefix &origin = event.origin();
        std::string clientNickName;

        getClientNickName(clientNickName);
        if ( origin.nick() == clientNickName )
        {
            removeChannel(event.channel());
        } else
        {
            removeUserFromChannel(origin.nick(), event.channel());
        }
    }
    void NetworkCache::receiveEvent(anp::irc::events::Topic &event)
    {
        setTopic(event.channel(), event.topic());
    }

    void NetworkCache::receiveEvent(anp::irc::events::Kick &event)
    {
        std::string clientNickName;

        getClientNickName(clientNickName);
        if ( event.kickedUser() == clientNickName )
        {
            removeChannel(event.channel());
        } else
        {
            removeUserFromChannel(event.kickedUser(), event.channel());
        }
    }

    void NetworkCache::receiveEvent(anp::irc::events::Quit &event)
    {
        anp::threading::Lock lock(m_impl->m_mutex);
        m_impl->removeUserFromAllChannels(event.origin().nick());
    }

    void NetworkCache::receiveEvent(anp::irc::events::Nick &event)
    {
        anp::threading::Lock lock(m_impl->m_mutex);
        m_impl->updateNick(event.origin().nick(), event.nick());
    }

    void NetworkCache::receiveEvent(anp::irc::events::Command &event)
    {
        if ( event.command() == "MODE" )
        {
            unsigned int currModeParamIndex = 2;
            const std::string &target = event.param(0);
            const std::string &modes = event.param(1);
            const char *cmodes = modes.c_str();
            char mchar;
            enum Action { ADD, REMOVE };
            Action currAction;

            std::stringstream ss;

            while ( (mchar = *cmodes) != 0 )
            {
                switch ( mchar )
                {
                case '+':
                    currAction = ADD;
                    break;
                case '-':
                    currAction = REMOVE;
                    break;
                default:
                    if ( modeconv_validMode(mchar) )
                    {
                        ChannelUserRelation &curel =
                            m_impl->cuRel(target, event.param(currModeParamIndex++));
                        std::string::size_type ret = curel.m_modes.find(mchar);
                        if ( std::string::npos == ret && currAction == ADD )
                        {
                            curel.m_modes += mchar;
                        } else if ( std::string::npos != ret
                                    && currAction == REMOVE )
                        {
                            curel.m_modes.erase(ret, 1);
                            // Assume there were no duplicates
                        }
                    } else
                    {
                        ss.str("");
                        ss.clear();
                        ss << "MODE parser: Unrecognized modechar: '" << mchar << "'";
                        ANPLOGE("libfircc-cache", ss.str());
                    }
                    break;
                }

                ++cmodes;
            }
        }
    }

    void NetworkCache::handle_RPL_NAMREPLY(const std::string &channel,
                                           const std::string &userlist)
    {
        using tokenizer::tokenize;
        std::string nick, userlistCopy = userlist;
        char mode;
        bool keepGoing = true;

        while ( keepGoing ) {
            // nick = [@|+|nothing]nickname
            keepGoing = tokenize(nick, // note, reusing var nick
                                 userlistCopy,
                                 " ");

            mode = modeconv_parseNick(nick);

            // Stupid temporary fix.
            // Parsing should be fixed instead.
            if ( nick != "" && nick != " " )
            {
                addUserToChannel(nick, "", "", channel, mode);
            }
        }
    }
}
}
