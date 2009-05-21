#ifndef _PLUGIN_MANAGER_H_
#define _PLUGIN_MANAGER_H_

#include <fstream>
#include "messagesender.h"
#include "plugin.h"
#include "pluginexecuter.h"

namespace irc {
	class NetworkCache;
}

struct Task {
	CPlugin *plugin; // Perhaps not needed!? WTF! only func and args needed !? :S
	void *func;
	int argc;
	char **argv;
};

class CPluginManager {
public:
	CPluginManager(CMessageSender &messageSender, CEventProcessor &eventProcessor, int agentID, const char *pluginFile, irc::NetworkCache *cache);
	~CPluginManager();

	// Events
	void onPing(const std::string &sender); // Here sender is expected to be a server
	void onPrivMsg(const std::string &sender, const std::string &target, const std::string &message);
	void onJoin(const std::string &nick, const std::string &channel);
	void onPart(const std::string &nick, const std::string &channel);

	// Utilities..
	bool waitForJob(Task *t);
	void stopWaitingForJob();

	void unload(CPlugin *plugin);
protected:

private:
	char *stdStr2ansiStr(const std::string &stdStr);

	CMessageSender &m_messageSender;
	CEventProcessor &m_eventProcessor;
	//CDataKeeper &m_dataKeeper

//	CPlugin m_tempREMOVE;
	std::vector<CPlugin *> m_plugins;
	std::vector<CPlugin *> m_eventListeners[CPlugin::EVENT_COUNT];
//	std::vector<CPlugin *> m_privMsgListeners;
//	std::vector<CPlugin *> m_pingListeners;
//	std::vector<CPlugin *> m_joinListeners;
//	std::vector<CPlugin *> m_partListeners;
	std::queue<Task> m_jobQueue;
	CEvent m_newJobEvent;
	CMutex m_jobQueueMutex;

	enum {
		EXECUTER_COUNT=1
	};

	CPluginExecuter m_pluginExecuters[EXECUTER_COUNT];

	int m_agentID;
};

#endif // _PLUGIN_MANAGER_H_