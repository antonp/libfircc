#include "messagesender.h"

CMessageSender::CMessageSender(CTCPConnection *connection, CEventProcessor *eventHandler, int agentID):
#ifdef _DEBUG
CActiveObject("CMessageSender"),
#endif
m_connection(connection),
m_eventHandler(*eventHandler),
m_agentID(agentID),
m_anyoneWaitingForLastMessage(false) {

	m_thread.resume();
}

void CMessageSender::initThread() {
#ifdef _DEBUG
	::SetThreadName(-1, "CMessageSender");
#endif
}

void CMessageSender::run() {
	std::string msg;
	while ( !m_isDying ) {
		// Wait for tasks
		m_newMessageEvent.wait();

		while ( 1 ) {
			{
				// Wait for and reserve the queue
				CLock lock(m_messageQueueMutex);
			
				if ( m_messages.empty() ) {
					break;
				} else {
					msg = m_messages.front();
					m_messages.pop();
				}
			}
			// Ok, we have task t and the queue isn't
			// reserved any more, so we can take all the
			// time we need to perform this task
			m_connection->Send(msg);
//			msg = "Sent: " + msg; // temporary
//			m_eventHandler.addEvent(m_agentID, -1, -1, msg);
		}
		bool temp=false;
		{
			CLock lock(m_anyoneWaitingForLastMessageMutex);
			temp = m_anyoneWaitingForLastMessage;
		}
		if ( temp ) {
			m_lastMessageEvent.release();
		}
	}
}

void CMessageSender::flushThread() {
	m_newMessageEvent.release();
	m_lastMessageEvent.release();
}

void CMessageSender::sendMessage(const std::string &message) {
	{
		CLock lock(m_messageQueueMutex);
		m_messages.push(message);
	}
	m_newMessageEvent.release();
}

void CMessageSender::waitForLastMessage() {
	bool empty = false;
	{
		CLock lock(m_messageQueueMutex);
		empty = m_messages.empty();
	}
	if ( !empty ) {
		{
			CLock lock(m_anyoneWaitingForLastMessageMutex);
			m_anyoneWaitingForLastMessage = true;
		}
		m_lastMessageEvent.wait();
		{
			CLock lock(m_anyoneWaitingForLastMessageMutex);
			m_anyoneWaitingForLastMessage = false;
		}
	}
}