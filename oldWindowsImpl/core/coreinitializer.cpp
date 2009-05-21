#include "coreinitializer.h"

CCoreInitializer::CCoreInitializer(CAgent **ppAgent, EVENTHANDLER eventHandler):
#ifdef _DEBUG
CActiveObject("CCoreInitializer"),
#endif
m_settingTree(NULL),
//m_ppAgent(ppAgent), // no longer needed, agents are now stored in a list
m_eventProcessor(eventHandler) {

	m_thread.resume();
}

void CCoreInitializer::initThread() {
#ifdef _DEBUG
	::SetThreadName(-1, "CCoreInitializer");
#endif
	// init the entire shit
//	m_outputProcessor << firc::loadmsg << "-------------------------\n";
	m_eventProcessor.addEvent(-1, -1, -1, firc::loadmsg);

//	m_outputProcessor << "loading settings from config.cfg...\n";
//	if ( !SETTINGS->loadFromFile() ) {
		//m_out("failed to open config.cfg for reading...\n");
//		m_outputProcessor << "failed to open config.cfg for reading...\n";
//		return;
//	}
//	m_outputProcessor << "loading settings from settings.xml\n";
	m_eventProcessor.addEvent(-1, -1, -1, "loading settings from settings.xml\n");
	if ( settingtree::loadTreeFromXML("settings.xml", &m_settingTree) != 1 ) {
//		m_outputProcessor << "failed to load settings.xml...\n";
		m_eventProcessor.addEvent(-1, -1, -1, "failed to load settings.xml...\n");
		Sleep(10000);
		return;
	}

	// Create all agents (which are active objects which will start executing immideatly)
	// so perhaps think about protecting the optionmanager somehow.. perhaps one optionmanager per agent..
	// or mutexes and shit
	settingtree::CGroupNode *agent = NULL;
	int size = 0;
	std::list<settingtree::CNode *> *children = m_settingTree->getChildren(size);
	for ( int i=0; i<size; ++i ) {
		for ( std::list<settingtree::CNode *>::iterator j=children[i].begin(); j != children[i].end(); j++ ) {
			if ( (*j)->getType() == settingtree::CNode::AGENT ) {
				agent = (settingtree::CGroupNode *)(*j);

				std::string bot_nick, bot_realname;
				agent->getString("bot_nick", bot_nick);
				agent->getString("bot_realname", bot_realname);

				m_agentLabels.push_back(*agent->getLabel());
				m_agents.push_back(	new CAgent(&m_eventProcessor,
									bot_nick,
									bot_realname,
									agent, m_agents.size()));
				
			}
		}
	}
//	*m_ppAgent = new CAgent(&m_outputProcessor, SETTINGS->getString("bot_nick"), SETTINGS->getString("bot_realname"));

	//a->m_thread.resume(); should be done AFTER every agent has been created to avoid conflicts
}

void CCoreInitializer::run() {
	// sleep until we receive the shut down event
	//m_out("I'm sleeping until I get the shutdown event\n");
	m_eventProcessor.addEvent(-1, -1, -1, "CoreInitializer: I'm sleeping until I get the shutdown event\n");
	m_shutDownEvent.wait();
	//m_out("got it, trying to delete the agent now\n");
	//m_eventProcessor.addEvent(-1, -1, -1, "CoreInitializer: Got the shutdown event, deleting agents...\n");

	// and when it's time, destroy everything! :D
	for ( std::vector<CAgent *>::iterator i=m_agents.begin(); i != m_agents.end(); i++ ) {
		(*i)->deinit();
	}
	//m_eventProcessor.addEvent(-1, -1, -1, "CoreInitializer: Test (shutdown sequence)\n");
	m_eventProcessor.waitUntilReadyToBeKilled(); // todo experimental BUT NEEDED check to verify it actually works..
	m_eventProcessor.kill();

	for ( std::vector<CAgent *>::iterator i=m_agents.begin(); i != m_agents.end(); i++ ) {
		delete (*i);
		(*i) = NULL;
	}
	m_cleanupDoneEvent.release();
}

void CCoreInitializer::flushThread() {
	m_shutDownEvent.release();
}

int CCoreInitializer::getAgentCount() {
	return m_agentLabels.size();
}

void CCoreInitializer::getAgentLabel(int id, std::string &dest) {
	// Avoid unnessecary out of bounds errors.. probably a good idea here
	if ( id >= 0 && id < m_agentLabels.size() ) {
		dest = m_agentLabels[id];
	} else if ( id == -1 ) {
		dest = "system";
	} else
		dest = "invalid id";

	// TODO: check the real agent labels if they have changed.. only do it this way if the id can't be matched
}

void CCoreInitializer::waitUntilCleanupDone() {
	m_cleanupDoneEvent.wait();
}