#ifndef _MESSAGE_RECEIVER_H_
#define _MESSAGE_RECEIVER_H_

#include <sstream>
#include "../../multitaskinglib/activeobject.h"
//#include "outputprocessor.h"

class CPluginManager;
class CTCPConnection;
class CEventProcessor;

namespace irc {
	class NetworkCache;
}

class CMessageReceiver: public CActiveObject {
public:
	CMessageReceiver(CTCPConnection *connection, CEventProcessor *eventProcessor, /*CMessageSender *messageSender*/CPluginManager *pluginManager, int agentID, irc::NetworkCache *cache);

	void initThread();
	void run();
	void flushThread();
private:
	CTCPConnection *m_connection;
	CEventProcessor &m_eventProcessor;
	CPluginManager *m_pluginManager;
	irc::NetworkCache *m_cache;

	int m_agentID;
};

#endif // _MESSAGE_RECEIVER_H_