#ifndef _EVENTPROCESSOR_H_
#define _EVENTPROCESSOR_H_

#include <queue>
#include <string>
#include "../../multitaskinglib/activeobject.h"
#include "../../multitaskinglib/event.h"
#include "../../multitaskinglib/lock.h"
#include "frontend_interface.h"

struct Event {
	int agentID;
	int type;
	int source;
	// What is needed? is three strings a good idea? or just a void pointer?
	std::string str1;
	std::string str2;
	std::string str3;
};

class CEventProcessor: public CActiveObject {
public:
	CEventProcessor(EVENTHANDLER eventHandler);
	~CEventProcessor() {
		// This is only here for debugging purposes.. to see when the destructor is called..
	}

	void initThread();
	void run();
	void flushThread();
	void waitUntilReadyToBeKilled();

	void addEvent(	int agentID,
					int type,
					int source,
					const std::string &str1,
					const std::string &str2="",
					const std::string &str3=""	);
private:
	std::queue<Event *> m_eventQueue;
	CEvent m_newEventEvent;
	CEvent m_readyToBeKilledEvent;
	CMutex m_eventQueueMutex;
	EVENTHANDLER m_eventHandler;
};

#endif // _EVENTPROCESSOR_H_