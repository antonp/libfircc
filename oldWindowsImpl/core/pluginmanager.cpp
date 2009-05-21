#include "pluginmanager.h"

CPluginManager::CPluginManager(CMessageSender &messageSender, CEventProcessor &eventProcessor, int agentID, const char *pluginFile, irc::NetworkCache *cache):
m_messageSender(messageSender)/*,
m_dataKeeper(dataKeeper)*/,
m_eventProcessor(eventProcessor),
m_agentID(agentID) {
	/* NOTE:
		Put this in a new function like "loadPlugins" or something
		*************************/
	std::ifstream fin;
	std::string line;
	char buffer[100];

	fin.open(pluginFile, std::ios_base::in);
	if ( !fin.is_open() )
//		return false;
		return;

	while ( !fin.eof() ) {
		fin.getline(buffer, 100);
		line = buffer;

		CPlugin *plugin = new CPlugin(messageSender, eventProcessor, cache, agentID);
		plugin->loadFromFile(line.c_str());
		plugin->init();
		m_plugins.push_back(plugin);
//		if ( plugin->wantsEvent(CPlugin::PRIV

		for ( unsigned int i=0; i<CPlugin::EVENT_COUNT; ++i ) {
			if ( plugin->wantsEvent(i) ) {
				m_eventListeners[i].push_back(plugin);
			}
		}
	}

	// We're done with the file
	fin.close();
//	return true;

	// Start up the worker threads
	for ( int i=0; i<EXECUTER_COUNT; i++ ) {
		m_pluginExecuters[i].setPluginManager(this);
		//m_pluginExecuters[i].initThread(); called implicitly anyway?
	}

	return;
}

CPluginManager::~CPluginManager() {
	for ( int i=0; i<m_plugins.size(); ++i ) {
		delete m_plugins[i];
	}
	m_plugins.clear();

	// MAYBE this should be done before plugin cleanup
	for ( unsigned int i=0; i<EXECUTER_COUNT; i++ ) {
		m_pluginExecuters[i].kill();
	}
}

void CPluginManager::onPing(const std::string &sender) {
	// Cheating way.. perhaps it shouldn't be hardcoded this early
	m_messageSender.sendMessage("PONG\r\n");

	Task t;
	int size = m_eventListeners[CPlugin::PING].size();
	{
		CLock lock(m_jobQueueMutex);
		for ( int i=0; i<size; i++ ) {
			t.plugin = m_eventListeners[CPlugin::PING][i];

			t.func		= m_eventListeners[CPlugin::PING][i]->m_pingCallback;
			t.argc		= 2;
			t.argv		= new char *[t.argc];
			t.argv[0]	= (char *)m_eventListeners[CPlugin::PING][i]->m_pluginIndex;
			t.argv[1]	= stdStr2ansiStr(sender);
			m_jobQueue.push(t);
		}
	}
	m_newJobEvent.release(); // But what if there are no privMsgListeners? :S
}

void CPluginManager::onPrivMsg(const std::string &sender, const std::string &target, const std::string &message) {
	Task t;

	int size = m_eventListeners[CPlugin::PRIVMSG].size();
	{
		CLock lock(m_jobQueueMutex);
		for ( int i=0; i<size; i++ ) {
			t.plugin = m_eventListeners[CPlugin::PRIVMSG][i];
			// args ????? in plugin or in task ?? probably in task! but think!

			t.func		= m_eventListeners[CPlugin::PRIVMSG][i]->m_privMsgCallback;
			t.argc		= 4;
			t.argv		= new char *[t.argc];
			t.argv[0]	= (char *)m_eventListeners[CPlugin::PRIVMSG][i]->m_pluginIndex/*->m_privMsgCallback*/; // Should only be CPlugin pointer?
			t.argv[1]	= stdStr2ansiStr(sender);
			t.argv[2]	= stdStr2ansiStr(target);
			t.argv[3]	= stdStr2ansiStr(message);
			m_jobQueue.push(t);
		}
	}
	m_newJobEvent.release(); // might as well wait with the event until we've added all tasks
	
	//	int size = m_privMsgListeners.size();
//	for ( int i=0; i<size; i++ ) {
		// Call onPrivMsg() or something on every listener
//	}
}

void CPluginManager::onJoin(const std::string &nick, const std::string &channel) {
	Task t;
	int size = m_eventListeners[CPlugin::JOIN].size();
	{
		CLock lock(m_jobQueueMutex);
		for ( int i=0; i<size; i++ ) {
			t.plugin = m_eventListeners[CPlugin::JOIN][i];

			t.func		= m_eventListeners[CPlugin::JOIN][i]->m_joinCallback;
			t.argc		= 3;
			t.argv		= new char *[t.argc];
			t.argv[0]	= (char *)m_eventListeners[CPlugin::JOIN][i]->m_pluginIndex;
			t.argv[1]	= stdStr2ansiStr(nick);
			t.argv[2]	= stdStr2ansiStr(channel);
			m_jobQueue.push(t);
		}
	}
	m_newJobEvent.release(); // But what if there are no privMsgListeners? :S
}

void CPluginManager::onPart(const std::string &nick, const std::string &channel) {
	Task t;
	int size = m_eventListeners[CPlugin::PART].size();
	{
		CLock lock(m_jobQueueMutex);
		for ( int i=0; i<size; i++ ) {
			t.plugin = m_eventListeners[CPlugin::PART][i];

			t.func		= m_eventListeners[CPlugin::PART][i]->m_partCallback;
			t.argc		= 3;
			t.argv		= new char *[t.argc];
			t.argv[0]	= (char *)m_eventListeners[CPlugin::PART][i]->m_pluginIndex;
			t.argv[1]	= stdStr2ansiStr(nick);
			t.argv[2]	= stdStr2ansiStr(channel);
			m_jobQueue.push(t);
		}
	}
	m_newJobEvent.release(); // But what if there are no privMsgListeners? :S
}

bool CPluginManager::waitForJob(Task *t) {
	/***************************************
	Makes the calling thread wait for a job
	Also, this function is overly complicated, sorry :P
	***************************************/
/*
	bool queueEmpty = false;
	bool firstIteration = true;

	do {
		if ( !firstIteration ) {
			// This is not the first iteration, so now the jobQueueMutex is locked by us
			m_jobQueueMutex.release();
		}
		firstIteration = false; // MISSTÄNKT! (stod true innan)

		// Lock the queue temporarily and check if it's empty
		{
			CLock lock(m_jobQueueMutex);
			queueEmpty = m_jobQueue.empty();
		}
		if ( queueEmpty ) {
			// Yep it's empty, wait for it to get populated
			m_newJobEvent.wait();
		}

		// The newJobEvent was released! Now every Executer is trying to get it
		m_jobQueueMutex.acquire();
		queueEmpty = m_jobQueue.empty();

		// Now that the queue is once again locked...
		// ... did we get it first?
	} while ( queueEmpty );

	// Yes we did!
	*t = m_jobQueue.front();
	m_jobQueue.pop();
	// Release the queue again
	m_jobQueueMutex.release();

	// Now the calling thread can exit this function and start executing :)
	*/

	///////////////////////////////
	// Second implementation (actually more like fifth)

	// Lock/reserve the queue
	//CLock lock(m_jobQueueMutex);
	m_jobQueueMutex.acquire();

	// Wait for a job to be added to the queue (but what if there are already jobs in the queue?)
	if ( m_jobQueue.empty() ) {
		m_jobQueueMutex.release();
		m_newJobEvent.wait();
		// Here, just in between the wait and acquire calls, we might lose a job to some other thread...
		m_jobQueueMutex.acquire();
		
		// ... So, we need to check that the queue isn't empty again
		if ( m_jobQueue.empty() ) {
			m_jobQueueMutex.release(); // OMG !!! this caused deadlocks! (forgot to release)
			return false; // If it is, tell the calling thread it didn't get any job, and it will try again.
		}
	}

	*t = m_jobQueue.front();
	m_jobQueue.pop(); // pretty important.. forgot this at first :P
	m_jobQueueMutex.release();
	
	return true;
}

void CPluginManager::stopWaitingForJob() {
	m_newJobEvent.release();
}

void CPluginManager::unload(CPlugin *plugin) {
	// TODO: Mutex crap for lists and stuff, so that they aren't read from while changing them

	// Remove it from all lists
	for ( unsigned int i=0; i<CPlugin::EVENT_COUNT; ++i ) {
		if ( plugin->wantsEvent(i) ) {
			// Do a linear search for the plugin in this list
			for ( unsigned int j=0; j<m_eventListeners[i].size(); ++j ) {
				if ( m_eventListeners[i][j] == plugin ) {
					// Found it, so remove it from this list
					m_eventListeners[i].erase(m_eventListeners[i].begin()+j);
				}
			}
		}
	}

	// Notify other plugin managers (running on other agents/networks)
//	for ( unsigned int i=0; i<antal plugin managers; ++i ) {
//		plugin managers[i]->notifyUnload(filename/HMODULE on windows..?)
//	}

	// Ok now we just need to unload it
	for ( unsigned int i=0; i<m_plugins.size(); ++i ) {
		if ( m_plugins[i] == plugin ) {
			// Match. Now unload it
			delete plugin;
			// Reserve the spot in the list
			m_plugins[i] = plugin = NULL;
		}
	}

}

char *CPluginManager::stdStr2ansiStr(const std::string &stdStr) {
	char *str = new char[stdStr.size()+1];
	strcpy(str, stdStr.c_str());

	return str;
}