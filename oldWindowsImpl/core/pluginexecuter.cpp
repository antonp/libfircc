#include "pluginexecuter.h"
#include "pluginmanager.h"

CPluginExecuter::CPluginExecuter():
#ifdef _DEBUG
CActiveObject("CPluginExecuter"),
#endif
m_pluginManager(NULL) {
//	m_thread.resume();
}

CPluginExecuter::~CPluginExecuter() {

}

void CPluginExecuter::setPluginManager(CPluginManager *pm) {
	m_pluginManager = pm;
	// Perhaps not too logical to do this here.. but what the heck
	m_thread.resume();
}

void CPluginExecuter::run() {
	Task t;

	while ( !m_isDying ) {
		// Pull a job off the queue
		if ( m_pluginManager->waitForJob(&t) == false )
			continue; // Someone else took the job right in front of our eyes :/
			//(OR there were never a job because the shutdown process has started)

		// Now that we have our job/task t, run it!
		t.plugin->callEventHandler(t.func, t.argc, t.argv);

		// Free up memory ( i=0 is reserved for the CPlugin pointer... )
		for ( unsigned int i=1; i<t.argc; ++i ) {
			delete[] t.argv[i];
		}
		delete[] t.argv;

		// If the plugin is marked as wanting to unload (having called the plugin_unload function)
		// tell the plugin manager about it!
//		if ( t.plugin->wantsToUnload() ) {
//			m_pluginManager->unload(t.plugin);
//		}
	}
}

void CPluginExecuter::flushThread() {
	// Release from waiting state so it can finish!!
	m_pluginManager->stopWaitingForJob();
}