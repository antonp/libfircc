#include "frontend_interface.h"
#include "coreinitializer.h"

#ifndef NULL
#define NULL 0
#endif

//OUTPUTHANDLER output = NULL;
EVENTHANDLER eventHandler = NULL;
CAgent *a = NULL; // WTF is this? remove..
CCoreInitializer *coreInitializer = NULL;

//void FIRC_COREAPI firc_setOutputHandler(OUTPUTHANDLER o) {
//	output = o;
//}

void FIRC_COREAPI firc_setEventHandler(EVENTHANDLER e) {
	eventHandler = e;
}

void FIRC_COREAPI firc_processInput(char *s) {
	// TODO: forward to plugin system
	// HINT: a source needs to be added just like in the output handler, to find the right agent
//	if ( output != NULL )
//		output(s, -1);
	eventHandler(-1, -1, -1, s, NULL, NULL);
}

class CCoreKiller: public CActiveObject {
public:
	CCoreKiller()
#ifdef _DEBUG
		:CActiveObject("CCoreKiller")
#endif
	{ }

	void startThread() {
		m_thread.resume();
	}

	void initThread() { }
	void run() {
		coreInitializer->waitUntilCleanupDone();
		coreInitializer->kill();
		delete coreInitializer;
		coreInitializer = NULL;
		eventHandler(-1, -2, -1, "Core cleanup done\n", NULL, NULL);
	}
	void flushThread() { }
} g_coreKiller;

void FIRC_COREAPI firc_init() {
	coreInitializer = new CCoreInitializer(&a, eventHandler); // remove 'a' here.. it's the old agent global crap
	g_coreKiller.startThread();
}

void FIRC_COREAPI firc_exit() {
	if ( coreInitializer != NULL ) {
		coreInitializer->flushThread();
		//Sleep(5000);
	}
}

int FIRC_COREAPI firc_getAgentCount() {
	return coreInitializer->getAgentCount();
}
void FIRC_COREAPI firc_getAgentLabel(char *dest, int maxsize, int id) {
	std::string temp;
	coreInitializer->getAgentLabel(id, temp);

	strcpy_s(dest, maxsize-1, temp.c_str());
}