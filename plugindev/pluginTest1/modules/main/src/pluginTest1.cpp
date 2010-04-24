#include <basedefs.h>
#include <iostream>
#include <sstream>
#include <string>

#include <networkmanager_frontend.h>
#include <networkmanagerevents.h>

using namespace anp;
using namespace anp::firc;
using namespace anp::firc::events;

using std::cout;
using std::endl;

class TestEventHandler: public ISubscriber<NewSession>,
					// public ISubscriber<RemovedSession>,
//					public ISubscriber<Join>,
//					public ISubscriber<Part>,
//					public ISubscriber<PrivMsg>,
//					public ISubscriber<Topic>,
					public ISubscriber<NumericReply>
{
public:
	void receiveEvent(NewSession &event)
	{
		cout << "[pluginTest1] <-] NewSession event received!" << endl;
		event.session().eventDispatcherNumericReply().subscribe(this);
	}
	void receiveEvent(NumericReply &event)
	{
		cout << "[pluginTest1 <-] Numeric reply " << event.command()
			<< " received. (" << event.origin().prefix() << ')' << endl;
	}
} g_handla;

extern "C" uint32 pluginInit(
	EventDispatcher<
		events::ISubscriber<events::NewSession>,
		events::NewSession
	> &newSessionDispatcher,
	void *appContext
)
{
	newSessionDispatcher.subscribe(&g_handla);
	cout << "pluginTest1.cpp: pluginInit()" << endl;

	return 1;
}

extern "C" void pluginDeinit()
{
	std::cout << "Good bye world from pluginTest1.cpp" << std::endl;
}

