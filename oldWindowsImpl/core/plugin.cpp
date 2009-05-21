#include "plugin.h"
#include "ircnetworkcache.h"
#include <string> // for sprintf

CPlugin::CPlugin(CMessageSender &messageSender, CEventProcessor &eventProcessor, irc::NetworkCache *cache, int agentID):
m_messageSender(messageSender),
m_eventProcessor(eventProcessor),
m_cache(cache),
m_agentID(agentID),
//m_wantsOnPrivMsg(false),
//m_wantsOnPing(false),
//m_wantsOnJoin(false),
//m_wantsOnPart(false),
m_pl_deinit(NULL) {
	for ( unsigned int i=0; i<EVENT_COUNT; ++i ) {
		m_wantsEvent[i] = false;
	}
}

CPlugin::~CPlugin() {
//	deinit();
//	free();
	for ( unsigned int i=0; i<m_plugins.size(); ++i ) {
		if ( m_plugins[i] == this ) {
			m_plugins[i] = NULL; // Reserve this spot to avoid ruining the indices of the other plugins
			m_reservationCount++;
		}
	}
}

int CPlugin::loadFromFile(const char *filename) {
	if ( (m_module = LoadLibrary(filename)) == NULL )
		return 0;
	// TODO: _WHEN_ plugins can be reloaded at runtime, there will be gaps in indices..
	// so look for unused indices somehow, perhaps through pluginmanagers array or something
	m_pluginIndex = m_nextPluginIndex++;
	// Lower 16bits holds the list index, upper 16 bits holds the agentID (which will be 0, 1, 2 and so on)
	m_pluginIndex |= (m_agentID << 16);
	CLock lock(m_pluginListMutex);
	if ( m_reservationCount > 0 ) {
		for ( unsigned int i=0; i<m_plugins.size(); ++i ) {
			if ( m_plugins[i] == NULL ) {
				// We found a reserved spot, let's take it
				m_plugins[i] = this;
				// Zero out the lower 16 bits and then OR it together with the index of this spot (i)
				m_pluginIndex = (m_pluginIndex & 0xffff0000) | i;
				m_reservationCount--;
			}
		}
	} else {
		m_plugins.push_back(this);
	}
	// TODO (2): Optimize vector capacity.. resize() :)

	return 1;
}

int CPlugin::init() {
	typedef int (__stdcall *PL_PREINIT)(void *(__stdcall *)(int), HPLUGININSTANCE);
	
	PL_PREINIT pl_preinit	= (PL_PREINIT)GetProcAddress(m_module, "plugin_preinit");
	PL_INIT pl_init			= (PL_INIT)GetProcAddress(m_module, "plugin_init");
	m_pl_deinit				= (PL_DEINIT)GetProcAddress(m_module, "plugin_deinit");
	
	//PL_ONPRIVMSG pl_onPrivMsg	= (PL_ONPRIVMSG)GetProcAddress(m_module, "onPrivMsg");

	if ( pl_init == NULL || pl_init == NULL )
		return 0;

	pl_preinit(getFunction, m_pluginIndex);//);
	pl_init(m_pluginIndex);

	return 1;
}

void CPlugin::deinit() {
	if ( m_pl_deinit != NULL )
		m_pl_deinit(m_pluginIndex);
	// Ensure that deinit only gets called once
	m_pl_deinit = NULL;
}

void CPlugin::free() {
	FreeLibrary(m_module);
}

void CPlugin::callEventHandler(void *func, int argc, char *argv[]) {
	// asm-lösning?

/*	__asm {
		movl ecx, argc
		movl eax, argv
		addl eax, ecx
		mul  eax, 4
arg_loop:
		addl eax, 4
		push eax
		loop arg_loop
	}
*/
	switch ( argc ) {
	case 0:
		((void (*)())func)(); // Cast and call func?
		break;
	case 2:
		((void (*)(char *, char *))func)(argv[0], argv[1]);
		break;
	case 3:
		((void (*)(char *, char *, char *))func)(argv[0], argv[1], argv[2]);
		break;
	case 4:
		((void (*)(char *, char *, char *, char *))func)(argv[0], argv[1], argv[2], argv[3]);
		break;
	}
}

void *CPlugin::getFunction(int func) {
	void *ret=NULL;

	switch (func) {
	case FRONTEND_PRINT:
		ret = (void *)frontend_print;
		break;
	case PLUGIN_UNLOAD:
		ret = (void *)plugin_unload;
		break;
	case IRC_SETPRIVMSGCALLBACK:
		ret = (void *)irc_setPrivMsgCallback;
		break;
	case IRC_SETPINGCALLBACK:
		ret = (void *)irc_setPingCallback;
		break;
	case IRC_SETJOINCALLBACK:
		ret = (void *)irc_setJoinCallback;
		break;
	case IRC_SETPARTCALLBACK:
		ret = (void *)irc_setPartCallback;
		break;
	case IRC_SAY:
		ret = (void *)irc_say;
		break;
	case IRC_SENDRAW:
		ret = (void *)CPlugin::irc_sendRaw;
		break;
	case IRC_ISBOTINCHANNEL:
		ret = (void *)CPlugin::irc_isBotInChannel;
		break;
	case IRC_ISUSERINCHANNEL:
		ret = (void *)CPlugin::irc_isUserInChannel;
		break;
	case IRC_GETCHANNELUSERCOUNT:
		ret = (void *)CPlugin::irc_getChannelUserCount;
		break;
	case IRC_GETTOPIC:
		ret = (void *)CPlugin::irc_getTopic;
		break;
	default:
		ret = NULL;
		break;
	}

	return ret;
}

void CPlugin::frontend_print(HPLUGININSTANCE handle, const char *text) {
	CPlugin *p = retrievePluginPointer(handle);
	p->m_eventProcessor.addEvent(p->m_agentID, -1, -1, text);
}

void CPlugin::plugin_unload(HPLUGININSTANCE handle, unsigned int options) {
	CPlugin *p = retrievePluginPointer(handle);
	
	// Add it to the list of plugins to unload
//	p->m_plugins
}

void CPlugin::irc_setPrivMsgCallback(HPLUGININSTANCE handle, PL_ONPRIVMSG callback) {
	/* IDEA: perhaps reserve an event type for debug messages?
	Then a debug message could be sent here for instance */

	// New impl.
	CPlugin *p = retrievePluginPointer(handle);
	p->m_wantsEvent[PRIVMSG] = ((p->m_privMsgCallback = callback) != NULL);
}

void CPlugin::irc_setPingCallback(HPLUGININSTANCE handle, PL_ONPING callback) {
	CPlugin *p = retrievePluginPointer(handle);
	p->m_wantsEvent[PING] = ((p->m_pingCallback = callback) != NULL);
}

void CPlugin::irc_setJoinCallback(HPLUGININSTANCE handle, PL_ONJOIN callback) {
	CPlugin *p = retrievePluginPointer(handle);
	p->m_wantsEvent[JOIN] = ((p->m_joinCallback = callback) != NULL);
}

void CPlugin::irc_setPartCallback(HPLUGININSTANCE handle, PL_ONPART callback) {
	CPlugin *p = retrievePluginPointer(handle);
	p->m_wantsEvent[PART] = ((p->m_partCallback = callback) != NULL);
}

void CPlugin::irc_say(HPLUGININSTANCE handle, const char *target, const char *message) {
	CPlugin *p = retrievePluginPointer(handle);
	p->m_messageSender.sendMessage(std::string("PRIVMSG ")+std::string(target)+" :"+message+"\r\n");
}

void CPlugin::irc_sendRaw(HPLUGININSTANCE handle, const char *rawCommand) {
	CPlugin *p = retrievePluginPointer(handle);
	std::string rawCmd = rawCommand; rawCmd += "\r\n";
	p->m_messageSender.sendMessage(rawCmd);
}

bool CPlugin::irc_isBotInChannel(HPLUGININSTANCE handle, const char *channel) {
	CPlugin *p = retrievePluginPointer(handle);

	return p->m_cache->isBotInChannel(channel);
}

bool CPlugin::irc_isUserInChannel(HPLUGININSTANCE handle, const char *channel, const char *user) {
	CPlugin *p = retrievePluginPointer(handle);

	return (p->m_cache->isUserInChannel(channel, user) == 1);
}

int CPlugin::irc_getChannelUserCount(HPLUGININSTANCE handle, unsigned int *dest, const char *channel) {
	CPlugin *p = retrievePluginPointer(handle);

	return p->m_cache->getChannelUserCount(dest, channel);
}

int CPlugin::irc_getTopic(HPLUGININSTANCE handle, char *dest, unsigned int destSize, const char *channel) {
	CPlugin *p = retrievePluginPointer(handle);

	// TODO: Now the string is copied to times, if getTopic() accepted a char * instead (OR returned a const pointer) it would only have to be copied once
	std::string topic;
	int result = p->m_cache->getTopic(&topic, channel);
	strcpy_s(dest, destSize, topic.c_str());
	
	return result;
}

// This could be inline.. for optimization..
CPlugin *CPlugin::retrievePluginPointer(unsigned int index) {
	// Note, VERY dependent on 32bits storage
	index &= 0xffff;
	CLock lock(m_pluginListMutex);
	return m_plugins[index];
}

std::vector<CPlugin *> CPlugin::m_plugins;
CMutex CPlugin::m_pluginListMutex;

unsigned int CPlugin::m_reservationCount = 0;
CPlugin::HPLUGININSTANCE CPlugin::m_nextPluginIndex = 0;