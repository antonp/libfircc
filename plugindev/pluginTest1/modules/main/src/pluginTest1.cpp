/*

Copyright (c) 2010, Anton Petersson < anton (at) anp.nu >
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <basedefs.h>
#include <iostream>
#include <sstream>
#include <string>

#include <inetwork.h>
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
	cout << "Good bye world from pluginTest1.cpp" << endl;
}

