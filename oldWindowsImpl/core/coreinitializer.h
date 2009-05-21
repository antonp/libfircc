#ifndef _CORE_INITIALIZER_H_
#define _CORE_INITIALIZER_H_

#include "../../multitaskinglib/activeobject.h"
#include "../../multitaskinglib/event.h"
#include "agent.h"
#include "frontend_interface.h"
#include "project_constants.h"
#include "eventprocessor.h"
//#include "outputprocessor.h"

#include "../../settingtreelib/settingtreeutility.h" // probably included in agent.h so remove this on sight

class CCoreInitializer: public CActiveObject {
public:
	CCoreInitializer(CAgent **ppAgent, EVENTHANDLER eventHandler); // will be a container later

	void initThread();
	void run();
	void flushThread();

	int getAgentCount();
	void getAgentLabel(int id, std::string &dest);

	void waitUntilCleanupDone();

private:
	settingtree::CGroupNode *m_settingTree;
	//CAgent **m_ppAgent;
	std::vector<CAgent *> m_agents;
	std::vector<std::string> m_agentLabels;

	CEvent m_shutDownEvent;
	CEvent m_cleanupDoneEvent;


	CEventProcessor m_eventProcessor;
};

#endif // _CORE_INITIALIZER_H_