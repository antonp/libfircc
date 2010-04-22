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

class EventHandler: public ISubscriber<NewSession>,
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
		event.session().eventDispatcherNumericReply().subscribe(this);
	}
	void receiveEvent(NumericReply &event)
	{
		cout << "[pluginTest1 <-] Numeric reply " << event.command()
			<< " received. (" << event.origin().prefix() << ')' << endl;
	}
} g_eventHandler;

uint32 pluginInit(
	EventDispatcher<
		events::ISubscriber<events::NewSession>,
		events::NewSession
	> &newSessionDispatcher,
	void *appContext
)
{
	newSessionDispatcher.subscribe(&g_eventHandler);

	return 1;
}

void pluginDeinit()
{
	std::cout << "Good bye world from pluginTest1.cpp" << std::endl;
}

