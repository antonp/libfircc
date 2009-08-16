#ifndef _IRCCHANNEL_H_
#define _IRCCHANNEL_H_

#include <basedefs.h>
#include <string>
#include <vector>

namespace irc {
	class User {
	public:
		User(
			const std::string &nickName,
			const std::string &user="n/a",
			const std::string &host="n/a"
		):
		m_nickName(nickName),
		m_user(user),
		m_host(host) {
			
		};
		// "Copy" constructor taking a pointer to a user (assumes the pointer is valid)
		User(const User *user):
		m_nickName(user->m_nickName),
		m_user(user->m_user),
		m_host(user->m_host) {

		}
		const std::string *getNickName() const { return &m_nickName; }
		const std::string *getUser() const { return &m_user; }
		const std::string *getHost() const { return &m_host; }
	private:
		// These three strings together is the entire nick!crap@host string
		std::string m_nickName;
		std::string m_user;
		std::string m_host;
	};

	class Channel {
	public:
		Channel(const std::string &name);
		Channel(const Channel &channel); // Copy constructor
		// TODO: Destructor

		// Information sync functions
		void addUser(const std::string &nickName, const std::string &user, const std::string &host);
		void addUser(const User *user);
		void removeUser(const std::string &nickName);
		void setTopic(const std::string &newTopic);

		// Information retrieval functions
		const std::string *getName() const { return &m_name; }
		unsigned int getUserCount() const { return m_users.size(); }
		const std::string *getTopic() const { return &m_topic; }
		int findUserByNickName(const std::string &nickName) const;
		int getUsers(const std::vector<User> **users) const;
		
		anp::Result getUserNickName(anp::uint32 index,
								const std::string **nickName);
		anp::Result getUserString(anp::uint32 index,
								const std::string **userString);
		anp::Result getUserHost(anp::uint32 index,
								const std::string **host);		

		enum {
			USER_NOTFOUND=-1
		};
	private:
		bool verifyUser(const std::string &nickName, const std::string &user, const std::string &host);
		bool verifyUser(const User *user);

		std::string m_name;
		std::string m_topic;

		std::vector<User> m_users; // Maybe change back to User * later
									// to avoid string copying...
	};
} // namespace irc

#endif // _IRCCHANNEL_H_
