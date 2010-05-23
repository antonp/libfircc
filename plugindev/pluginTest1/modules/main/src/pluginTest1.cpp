#include <basedefs.h>
#include <iostream>
#include <sstream>
#include <string>

#include <network_frontend.h>
#include <networkevents.h>
#include <networkeventdispatchers.h>
#include <networkfactory.h>

using namespace anp;
using namespace anp::firc;
using namespace anp::firc::events;

using std::cout;
using std::endl;

class TestEventHandler: public ISubscriber<NewNetwork>,
						public ISubscriber<RemovingNetwork>,
					// public ISubscriber<RemovedSession>,
//					public ISubscriber<Join>,
//					public ISubscriber<Part>,
//					public ISubscriber<PrivMsg>,
//					public ISubscriber<Topic>,
					public ISubscriber<NumericReply>
{
public:
	void receiveEvent(NewNetwork &event)
	{
		cout << "[pluginTest1] <-] NewSession event received!" << endl;
		event.network().eventDispatcherNumericReply().subscribe(this);
	}
	void receiveEvent(RemovingNetwork &event)
	{
		cout << "[pluginTest2] <-] RemovingNetwork event received!" << endl;
		event.network().eventDispatcherNumericReply().unsubscribe(this);
	}
	void receiveEvent(NumericReply &event)
	{
		cout << "[pluginTest1 <-] Numeric reply " << event.command()
			<< " received. (" << event.origin().prefix() << ')' << endl;
	}
} g_handla;

extern "C" uint32 pluginInit(
	NetworkFactory &networkFactory,
	void *appContext
)
{
	networkFactory.eventDispatcherNewNetwork().subscribe(&g_handla);
	networkFactory.eventDispatcherRemovingNetwork().subscribe(&g_handla);
	cout << "pluginTest1.cpp: pluginInit()" << endl;

	return 1;
}

extern "C" void pluginDeinit()
{
	std::cout << "Good bye world from pluginTest1.cpp" << std::endl;
}

