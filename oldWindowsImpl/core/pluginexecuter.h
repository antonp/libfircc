#ifndef _PLUGINEXECUTER_H_
#define _PLUGINEXECUTER_H_

#include "../../multitaskinglib/activeobject.h"
#include "../../multitaskinglib/event.h"
#include "../../multitaskinglib/lock.h"

class CPluginManager; // Forward declaration

class CPluginExecuter: public CActiveObject {
public:
	CPluginExecuter();
	~CPluginExecuter();

	void setPluginManager(CPluginManager *pm);

	void initThread() {
#ifdef _DEBUG
		::SetThreadName(-1, "CPluginExecuter");
#endif
	};
private:
	void run();
	void flushThread();

	CPluginManager *m_pluginManager;
};

#endif // _PLUGINEXECUTER_H_