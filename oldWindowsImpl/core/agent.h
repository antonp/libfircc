#ifndef _AGENT_H_
#define _AGENT_H_

#include <string>
#include <sstream>
#include <fstream>
#include <vector>

#include "ircnetworkcache.h"

class CTCPConnection;
class CEventProcessor;
class CMessageReceiver;
class CMessageSender;
class CPluginManager;

namespace settingtree {
	class CGroupNode;
}
namespace irc {
	class NetworkCache;
}

/*
	There will be one agent per server, but they may share resources
	such as admin registry. Also, they may exchange messages :)
*/

struct AdminData {
	std::string user;		// antpet
	std::string currNick;	// pepper-
	std::string currHost;	// hostname of the user
	std::string password;	// mypass
	unsigned short access;	// perhaps there'll be a few different levels of access
	bool isOn;				// true if the user is on IRC and authed with the bot
};

/*
class COutputHelper {
friend COutputHelper &operator<<(COutputHelper &o, const std::string &str);
friend COutputHelper &operator<<(COutputHelper &o, const char *str);
friend COutputHelper &operator<<(COutputHelper &o, char ch);
public:
	COutputHelper(CEventProcessor &out, int sourceID): m_out(out), m_sourceID(sourceID) { }
private:
	COutputProcessor &m_out;
	int m_sourceID;
};*/

class CAgent {
public:
	CAgent(	CEventProcessor *eventHandler,
			const std::string &nick,
			const std::string &realname,
			settingtree::CGroupNode *agentRoot,
			int agentID);
	~CAgent();

	void getLabel(std::string &dest);
	bool init();
	void deinit();

	bool loadChannels(const std::string &channelFile);
	bool loadAdmins(const std::string &adminFile);

	bool registerConnection();

	int join(const std::string &channel);
private:
	void parseAdminInfo(std::string line);

	CTCPConnection *m_connection;
	settingtree::CGroupNode *m_agentRoot;
	std::string m_nickname;
	std::string m_realname;
	std::string m_pongStr;
	std::string m_label;

	irc::NetworkCache m_cache;

	// vars that makes life easier
	std::string m_outStr;

	std::vector<std::string> m_channelsToJoin;
	std::vector<AdminData> m_adminRegistry;

	// the important stuff
	CMessageReceiver *m_messageReceiver;
	CMessageSender *m_messageSender;
	CEventProcessor &m_eventHandler;
	CPluginManager *m_pluginManager;

	int m_agentID;
};

#endif // _AGENT_H_