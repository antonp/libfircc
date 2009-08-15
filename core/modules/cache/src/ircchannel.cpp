#include "../inc/ircchannel.h"
#include <basedefs.h>

namespace firc
{
	typedef void *ChannelInfo;
}

namespace irc {
	Channel::Channel(const std::string &name): m_name(name) {

	}
	
	Channel::Channel(const Channel &channel):
	m_name(channel.m_name),
	m_topic(channel.m_topic),
	m_users(channel.m_users)
	{
		
	}

	void Channel::addUser(const std::string &nickName, const std::string &user, const std::string &host) {
		// Verify this user
		if ( verifyUser(nickName, user, host) == true ) {
			User newUser(nickName, user, host);
			m_users.push_back(newUser);
		} else {
			// Error? log?
		}
	}

	void Channel::addUser(const User *user) {
		if ( verifyUser(user) == true ) {
			// Copy this user before adding it to the channel
			User newUser(user);
			m_users.push_back(newUser);
		} else {
			// Error? log?
		}
	}

	void Channel::removeUser(const std::string &nickName) {
		int index = findUserByNickName(nickName);
		if ( index != USER_NOTFOUND ) {
			m_users.erase(m_users.begin()+index);
		}
	}

	int Channel::findUserByNickName(const std::string &nickName) const {
		int ret = USER_NOTFOUND;
		for ( unsigned int i=0; i<m_users.size(); ++i ) {
			if ( *m_users[i].getNickName() == nickName ) {
				// Match
				ret = (int)i;
			}
		}
		return ret;
	}
	
	int Channel::getUsers(const std::vector<User> **users) const
	{
		if ( NULL != users )
		{
			*users = &m_users;
			return 1;
		}
		return 0; // invalid param
	}

	bool Channel::verifyUser(const std::string &nickName, const std::string &user, const std::string &host) {
		return (nickName.empty() == false && nickName[0] != ' ');
	}
	
	bool Channel::verifyUser(const User *user) {
		return verifyUser(*user->getNickName(), *user->getUser(), *user->getHost());
	}

	void Channel::setTopic(const std::string &newTopic) {
		m_topic = newTopic;
	}
	
	firc::Result Channel::getUserNickName(firc::uint32 index,
						const std::string **nickName)
	{
		if ( NULL != nickName
			&& index < m_users.size() )
		{
			*nickName = m_users[index].getNickName();
			return firc::RES_OK;
		} else
		{
			return firc::RES_INVALID_PARAMETER;
		}
	}
	
	firc::Result Channel::getUserString(firc::uint32 index,
						const std::string **userString)
	{
		if ( NULL != userString
			&& index < m_users.size() )
		{
			*userString = m_users[index].getUser();
			return firc::RES_OK;
		} else
		{
			return firc::RES_INVALID_PARAMETER;
		}
	}

	firc::Result Channel::getUserHost(firc::uint32 index,
							const std::string **host)
	{
		if ( NULL != host
			&& index < m_users.size() )
		{
			*host = m_users[index].getHost();
			return firc::RES_OK;
		} else
		{
			return firc::RES_INVALID_PARAMETER;
		}		
	}

} // namespace irc
	
	///////////////////////////////
	// Public API functions below
	///////////////////////////////

namespace firc
{	
	extern "C"
	{
		
		/**
		 * @brief
		 * Retrieves the topic from a ChannelInfo object.
		 * 
		 * @param channelInfo
		 * ChannelInfo object to retrieve information from.
		 * 
		 * @param destTopic
		 * Destination string buffer that the topic will be written to.
		 * 
		 * @param maxTopicSize
		 * Max size in characters of the topic to be retrieved.
		 * 
		 * @remark
		 * The topic returned might not be up to date and may
		 * very well have changed since the ChannelInfo object
		 * was created. ChannelInfo objects are only snapshots
		 * of a channel at a point in time.
		 * 
		 * @return 
		 * RES_OK on success, error code on failure.
		 */
		Result channelInfoGetTopic(ChannelInfo channelInfo,
									int8 *destTopic,
									uint32 maxTopicSize)
		{
			Result res = RES_INVALID_PARAMETER;
			irc::Channel *channelSnapShot = (irc::Channel *)channelInfo;
			
			if ( NULL != channelSnapShot
				&& NULL != destTopic
				&& maxTopicSize > 1 )
			{
				const std::string *topic = channelSnapShot->getTopic();
				uint32 i = 0;
				
				--maxTopicSize;
				for ( ; i<maxTopicSize && i<topic->size(); ++i )
				{
					destTopic[i] = (*topic)[i];
				}
				destTopic[i] = 0;
				res = RES_OK;
			}
			
			return res;
		}

		/**
		 * @brief
		 * Retrieves the number of users from a ChannelInfo object.
		 * 
		 * @param channelInfo
		 * ChannelInfo object to retrieve information from.
		 * 
		 * @param[out] userCount
		 * uint32 that the user count will be written to.
		 * 
		 * @return
		 * RES_OK on success, error code on failure.
		 */		
		Result channelInfoGetUserCount(
								ChannelInfo channelInfo,
								uint32 *userCount)
		{
			Result res = RES_INVALID_PARAMETER;
			irc::Channel *channelSnapShot = (irc::Channel *)channelInfo;
			
			if ( NULL != channelSnapShot
				&& NULL != userCount )
			{
				*userCount = channelSnapShot->getUserCount();
				res = RES_OK;
			}
			
			return res;
		}

		/**
		 * @brief
		 * Retrieves the nick name of a user in a channel.
		 * 
		 * @param channelInfo
		 * ChannelInfo object to retrieve information from.
		 * 
		 * @param index
		 * Zero based index of the user in the list. Valid index are in
		 * the range [0..userCount].
		 * 
		 * @param destNickName
		 * Will point to the read-only nick name of the user.
		 * 
		 * @return
		 * RES_OK on success, an error code on failure.
		 */		
		Result channelInfoGetUserNick(
										ChannelInfo channelInfo,
										uint32 index,
										const int8 **destNickName)
		{
			Result res = RES_INVALID_PARAMETER;
			
			if ( NULL != channelInfo
				&& NULL != destNickName )
			{
				const std::string *nickName = NULL;
				res = ((irc::Channel *)channelInfo)->getUserNickName(index,
															&nickName);
				if ( firc::RES_OK == res )
				{
					*destNickName = nickName->c_str();
				}
			}
			return res;
		}

		/**
		 * @brief
		 * Retrieves the user string of a user in a channel.
		 * 
		 * @param channelInfo
		 * ChannelInfo object to retrieve information from.
		 * 
		 * @param index
		 * Zero based index of the user in the list. Valid index are in
		 * the range [0..userCount].
		 * 
		 * @param destUserString
		 * Will point to the read-only user string of the user. Can be
		 * seen as the user name or user identifier.
		 * 
		 * @return
		 * RES_OK on success, an error code on failure.
		 */		
		Result channelInfoGetUserString(
									ChannelInfo channelInfo,
									uint32 index,
									const int8 **destUserString)
		{
			Result res = RES_INVALID_PARAMETER;
			
			if ( NULL != channelInfo
				&& NULL != destUserString )
			{
				const std::string *userString = NULL;
				res = ((irc::Channel *)channelInfo)->getUserNickName(
														index,
														&userString);
				if ( firc::RES_OK == res )
				{
					*destUserString = userString->c_str();
				}
			}
			return res;
		}
		
		/**
		 * @brief
		 * Retrieves the host of a user in a channel.
		 * 
		 * @param channelInfo
		 * ChannelInfo object to retrieve information from.
		 * 
		 * @param index
		 * Zero based index of the user in the list. Valid index are in
		 * the range [0..userCount].
		 * 
		 * @param destHost
		 * Will point to the read-only user string of the user. Can be
		 * seen as the user name or user identifier.
		 * 
		 * @return
		 * RES_OK on success, an error code on failure.
		 */		
		Result channelInfoGetUserHost(
									ChannelInfo channelInfo,
									uint32 index,
									const int8 **destHost)
		{
			Result res = RES_INVALID_PARAMETER;
			
			if ( NULL != channelInfo
				&& NULL != destHost )
			{
				const std::string *host = NULL;
				res = ((irc::Channel *)channelInfo)->getUserHost(index,
															&host);
				if ( RES_OK == res )
				{
					*destHost = host->c_str();
				}
			}
			return res;
		}
	}
	

} // namespace firc
