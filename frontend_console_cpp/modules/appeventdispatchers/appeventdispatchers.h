#ifndef _APPEVENTDISPATCHERS_H_
#define _APPEVENTDISPATCHERS_H_

#include <networkmanagerevents.h>
#include <eventdispatcher.h>

namespace app
{
	struct EventDispatchers
	{
		anp::EventDispatcher<
			anp::firc::events::ISubscriber<events::NewSession>,
			events::NewSession
		> newSession;
	};
}

#endif // _APPEVENTDISPATCHERS_H_
