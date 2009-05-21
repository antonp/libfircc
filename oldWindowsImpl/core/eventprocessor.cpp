#include "eventprocessor.h"

CEventProcessor::CEventProcessor(EVENTHANDLER eventHandler):
#ifdef _DEBUG
CActiveObject("CEventProcessor"),
#endif
m_eventHandler(eventHandler) {
	m_thread.resume();
}

void CEventProcessor::initThread() {
#ifdef _DEBUG
	::SetThreadName(-1, "CEventProcessor");
#endif
}

void CEventProcessor::run() {
	Event *e = NULL;
	while ( m_isDying == 0 ) {
		// Wait for output
		m_newEventEvent.wait();
		m_readyToBeKilledEvent.reset();

		// When there are events to process, go through the queue
		while ( 1 ) {
			// Reserve the eventQueue so that it can't be
			// manipulated while we're reading from it.
			// This is also great as it gives the other
			// threads a chance to add new events
			// in between the reads
			{
				if ( m_isDying != 0 )
					break;
				CLock lock(m_eventQueueMutex);
				if ( m_eventQueue.empty() ) {
					break;
				} else {
					// This solution requires that we copy the string
					// before we can send it to the output function
					// in the frontend. This is easily solved if we
					// only save pointers to the string obejcts in the
					// queue
					e = m_eventQueue.front();
					m_eventQueue.pop();
				
				}
			}
			// At this point the lock has gone out of
			// scope, so if the event handler in the
			// frontend is slow/blocking, the output
			// queue can still grow
			m_eventHandler(e->agentID, e->type, e->source, e->str1.c_str(), e->str2.c_str(), e->str3.c_str());

			// Since every event is allocated on the heap when it's added
			// to the queue, we need to free the memory when we're done
			delete e;
			e = NULL;
		}
		m_readyToBeKilledEvent.release();
	}
}

void CEventProcessor::flushThread() {
	m_newEventEvent.release();
}

void CEventProcessor::addEvent(int agentID,
							   int type,
							   int source,
							   const std::string &str1,
							   const std::string &str2,
							   const std::string &str3)
{
	Event *e = new Event;
	
	e->agentID	= agentID;
	e->type		= type;
	e->source	= source;
	e->str1		= str1;
	e->str2		= str2;
	e->str3		= str3;

	{
		// Reserve the eventQueue so that
		// no other thread can manipulate it
		// while we are adding our event data
		CLock lock(m_eventQueueMutex);
		m_eventQueue.push(e);
	}

	// Let the output processor know there
	// are new entries in the queue
	m_newEventEvent.release();
}
							 
void CEventProcessor::waitUntilReadyToBeKilled() {
	m_readyToBeKilledEvent.wait();
}