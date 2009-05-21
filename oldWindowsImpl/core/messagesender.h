#ifndef _MESSAGE_SENDER_H_
#define _MESSAGE_SENDER_H_

#include <queue>
#include "../../multitaskinglib/activeobject.h"
#include "../../multitaskinglib/event.h"
#include "../../multitaskinglib/lock.h"
#include "tcpconnection.h"
#include "eventprocessor.h"

//struct Task {
//	int opcode;
//
//	enum {
//		JOIN
//	};
//
//	std::string params[4];
//};

class CMessageSender: public CActiveObject {
public:
	CMessageSender(CTCPConnection *connection, CEventProcessor *eventHandler, int agentID);

	void initThread();
	void run();
	void flushThread();

	void sendMessage(const std::string &message);

	void waitForLastMessage();
private:
	CTCPConnection *m_connection;
	CEventProcessor &m_eventHandler;
	std::queue<std::string> m_messages;
	CEvent m_newMessageEvent;
	CEvent m_lastMessageEvent;
	CMutex m_messageQueueMutex;
	CMutex m_anyoneWaitingForLastMessageMutex;
	bool m_anyoneWaitingForLastMessage;
	int m_agentID;
};

#endif // _MESSAGE_SENDER_H_