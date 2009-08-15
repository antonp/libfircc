#include "../ircnetworkcache.h"
#include "../inc/ircchannel.h"

namespace irc {

	void NetworkCache::init(const std::string &host, const std::string &botname) {
		m_host = host;
		m_botNickName = botname;
	}

	void NetworkCache::onJoin(const std::string &nickName, const std::string &user, const std::string &host, const std::string &channel) {
		if ( nickName == m_botNickName ) {
			m_activeChannels.push_back(new Channel(channel));
		} else {
			// Someone else joined the channel
			int c = findChannel(channel);
			if ( c > CHANNEL_NOTFOUND ) {
				m_activeChannels[c]->addUser(nickName, user, host);
			} else {
				// This should never happen, error!
			}
		}
	}

	// ##### Note: unnecessary to have user and host params?
	void NetworkCache::onPart(const std::string &nickName, const std::string &user, const std::string &host, const std::string &channel) {
		int c = findChannel(channel);
		if ( c > CHANNEL_NOTFOUND ) {
			if ( nickName != m_botNickName ) {
				// Someone else left the channel
				m_activeChannels[c]->removeUser(nickName);
			} else {
				// The bot left the channel
				removeChannel(c);
			}
		} else {
			// This should never happen, error!
		}
	}

	void NetworkCache::onNickChange(const std::string &oldNick, const std::string &newNick) {
		if ( oldNick == m_botNickName ) {
			m_botNickName = newNick;
		} else {
			// Some moron changed his name (todo)
		}
	}

	void NetworkCache::setTopic(const std::string &channel, const std::string &newTopic) {
		int c = findChannel(channel);
		if ( c != CHANNEL_NOTFOUND ) {
			m_activeChannels[c]->setTopic(newTopic);
		} else {
			// Error? I wouldn't receive a TOPIC message when not in the channel, but I might be able to get RPL_TOPIC
		}
	}

	void NetworkCache::addUser(const std::string &channel, const std::string &nickName, const std::string &user, const std::string &host) {
		int c = findChannel(channel);
		if ( c > CHANNEL_NOTFOUND ) {
			m_activeChannels[c]->addUser(nickName, user, host);
		} else {
			// weird.. perhaps the bot issued a request for the names of a channel it's not in.. not sure if it should store info about channels it's not in
		}
	}

	void NetworkCache::getBotNickName(std::string *dest) const {
		*dest = m_botNickName;
	}

	bool NetworkCache::isBotInChannel(const std::string &channel) const {
		int c = 0;
		if ( (c=findChannel(channel)) != CHANNEL_NOTFOUND ) {
			//m_activeChannels[c]->
			return true; // Looks a bit stupid right now, but I'm not sure the "cache" will only hold info about channels that it is in
		} else {
			return false;
		}
	}

	int NetworkCache::isUserInChannel(const std::string &channel, const std::string &nickName) const {
		int c = findChannel(channel);
		if ( c != CHANNEL_NOTFOUND ) {
			return (int)(m_activeChannels[c]->findUserByNickName(nickName) != Channel::USER_NOTFOUND);
		} else {
			return -1; // -1 = unknown (constant needed?)
		}
	}
	
	int NetworkCache::getChannelUserCount(unsigned int *dest, const std::string &channel) const {
		int c = findChannel(channel);
		if ( c != CHANNEL_NOTFOUND ) {
			*dest = m_activeChannels[c]->getUserCount(); // assumes dest is a valid pointer
			return 1;
		} else {
			return -1; // -1 = unknown (constant needed?)
		}
	}
	
	int NetworkCache::getTopic(std::string *dest, const std::string &channel) const {
		int c = findChannel(channel);
		if ( c != CHANNEL_NOTFOUND ) {
			*dest = *m_activeChannels[c]->getTopic();
			return 1;
		} else {
			return -1; // -1 = unknown channel (constant needed?)
		}
	}
	
	firc::Result NetworkCache::createChannelInfoFromChannelName(
		const std::string &channel,
		const void **channelInfo
	) const
	{
		using firc::uint32;
		
		int c = findChannel(channel);

		if ( c > CHANNEL_NOTFOUND ) {
			if ( NULL != channelInfo )
			{
				Channel *tempChannelInfo =
					new Channel(*m_activeChannels[c]);
				
				if ( NULL != tempChannelInfo )
				{
					*channelInfo = (void *)tempChannelInfo;
				} else
				{
					return firc::RES_MEMALLOC_FAILED;
				}
			}
		} else {
			return firc::RES_NOTFOUND;
		}
		return firc::RES_OK;
	}

	int NetworkCache::removeChannel(int index) {
		delete m_activeChannels[index];
		m_activeChannels.erase(m_activeChannels.begin()+index);

		return 1; // Unnessecary?
	}

	int NetworkCache::findChannel(const std::string &name) const {
		int ret = CHANNEL_NOTFOUND;
		for ( unsigned int i=0; i<m_activeChannels.size(); ++i ) {
			if ( *m_activeChannels[i]->getName() == name ) {
				// Match
				ret = (int)i;
			}
		}
		return ret;
	}
} // namespace irc
