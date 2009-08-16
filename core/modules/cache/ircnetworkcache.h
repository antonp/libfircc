#ifndef _IRCNETWORKCACHE_H_
#define _IRCNETWORKCACHE_H_

#include <string>
#include <vector>

#include <basedefs.h>

namespace irc {
	class User;
	class Channel;

/*//////////////////////////
TODO:
Mutex protection where needed
Store channels in a hash table instead to avoid O(n) linear searches
getChannel() should generate a copy of the specified channel which won't need any mutex protection or lookup
getUser() same as above
*///////////////////////////

	class NetworkCache {
	public:
		void init(const std::string &m_host, const std::string &botname);

		// Information sync functions
		void onJoin(const std::string &nickName, const std::string &user, const std::string &host, const std::string &channel);
		void onPart(const std::string &nickName, const std::string &user, const std::string &host, const std::string &channel);
		void onNickChange(const std::string &oldNick, const std::string &newNick);
		void addUser(const std::string &channel, const std::string &nickName, const std::string &user="n/a", const std::string &host="n/a");
		void setTopic(const std::string &channel, const std::string &newTopic);
		
		// Information retrieval functions
		void getBotNickName(std::string *dest) const;
		bool isBotInChannel(const std::string &channel) const;
		int isUserInChannel(const std::string &channel, const std::string &nickName) const;
		int getChannelUserCount(unsigned int *dest, const std::string &channel) const;
		int getTopic(std::string *dest, const std::string &channel) const;
		anp::Result createChannelInfoFromChannelName(
			const std::string &channel,
			const void **channelInfo
		) const;
			


		enum {
			CHANNEL_NOTFOUND=-1
		};
	private:
		int findChannel(const std::string &name) const;
		int removeChannel(int index);

		std::string m_host; // frosties.de.eu.gtanet.com for instance
		std::string m_botNickName;
		std::vector<Channel *> m_activeChannels;
	};
	
} // namespace irc

#endif // _IRCNETWORKCACHE_H_
