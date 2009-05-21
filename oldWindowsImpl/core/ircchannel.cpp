#include "ircchannel.h"

namespace irc {
	Channel::Channel(const std::string &name): m_name(name) {

	}

	void Channel::addUser(const std::string &nickName, const std::string &user, const std::string &host) {
		// Verify this user
		if ( verifyUser(nickName, user, host) == true ) {
			m_users.push_back(new User(nickName, user, host));
		} else {
			// Error? log?
		}
	}

	void Channel::addUser(const User *user) {
		if ( verifyUser(user) == true ) {
			// Copy this user before adding it to the channel
			m_users.push_back(new User(user));
		} else {
			// Error? log?
		}
	}

	void Channel::removeUser(const std::string &nickName) {
		int index = findUserByNickName(nickName);
		if ( index != USER_NOTFOUND ) {
			delete m_users[index];
			m_users[index] = NULL;
			m_users.erase(m_users.begin()+index);
		}
	}

	int Channel::findUserByNickName(const std::string &nickName) const {
		int ret = USER_NOTFOUND;
		for ( unsigned int i=0; i<m_users.size(); ++i ) {
			if ( *m_users[i]->getNickName() == nickName ) {
				// Match
				ret = (int)i;
			}
		}
		return ret;
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
} // namespace irc