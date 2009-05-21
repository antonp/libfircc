#ifndef _PLUGIN_H_
#define _PLUGIN_H_

#include <windows.h> // for LoadLibrary etc
#include "messagesender.h"
#include "../../plugin library/firc_plugindefs.h"
#include "../../plugin library/firc_plugin.h"

namespace irc {
	class NetworkCache;
}

class CPlugin {
friend class CPluginManager;

	typedef unsigned int HPLUGININSTANCE;
public:
	CPlugin(CMessageSender &messageSender, CEventProcessor &eventProcessor, irc::NetworkCache *cache, int agentID);
	~CPlugin();

	int loadFromFile(const char *filename);
	int init();
	void deinit();
	void free();

	enum Events {
		PRIVMSG=0,
		PING,
		JOIN,
		PART,

		EVENT_COUNT
	};

	bool wantsEvent(unsigned int eventID) { return m_wantsEvent[eventID]; }

	void callEventHandler(void *func, int argc, char *argv[]);
private:
	// Native function only used by plugin library to connect the plugin to the core
	static void * __stdcall getFunction(int func);

	// Native functions
	static void __stdcall frontend_print(HPLUGININSTANCE handle, const char *text);
	static void __stdcall plugin_unload(HPLUGININSTANCE handle, unsigned int options);
	static void __stdcall irc_setPrivMsgCallback(HPLUGININSTANCE handle, PL_ONPRIVMSG callback);
	static void __stdcall irc_setPingCallback(HPLUGININSTANCE handle, PL_ONPING callback);
	static void __stdcall irc_setJoinCallback(HPLUGININSTANCE handle, PL_ONJOIN callback);
	static void __stdcall irc_setPartCallback(HPLUGININSTANCE handle, PL_ONPART callback);
	static void __stdcall irc_say(HPLUGININSTANCE handle, const char *target, const char *message);
	static void __stdcall irc_sendRaw(HPLUGININSTANCE handle, const char *rawCommand);
	static bool __stdcall irc_isBotInChannel(HPLUGININSTANCE handle, const char *channel);
	static bool __stdcall irc_isUserInChannel(HPLUGININSTANCE handle, const char *channel, const char *user);
	static int	__stdcall irc_getChannelUserCount(HPLUGININSTANCE handle, unsigned int *dest, const char *channel);
	static int	__stdcall irc_getTopic(HPLUGININSTANCE handle, char *dest, unsigned int destSize, const char *channel);

	static CPlugin *retrievePluginPointer(unsigned int index);

	HMODULE m_module;
	CMessageSender &m_messageSender;
	CEventProcessor &m_eventProcessor;
	irc::NetworkCache *m_cache;

	int m_agentID;

	// Index in the pluginmanager's list of plugins, used to avoid use of std::maps, making handle <-> plugin mapping O(1)
	HPLUGININSTANCE m_pluginIndex;
	static HPLUGININSTANCE m_nextPluginIndex;
	static unsigned int m_reservationCount;
	static std::vector<CPlugin *> m_plugins;
	static CMutex m_pluginListMutex;

	bool m_wantsEvent[EVENT_COUNT];

	PL_ONPRIVMSG m_privMsgCallback;
	PL_ONPING m_pingCallback;
	PL_ONJOIN m_joinCallback;
	PL_ONPART m_partCallback;
	
	typedef int (__stdcall *PL_INIT)(HPLUGININSTANCE handle);
	typedef int (__stdcall *PL_DEINIT)(HPLUGININSTANCE handle);
	PL_DEINIT m_pl_deinit;

};

#endif // _PLUGIN_H_