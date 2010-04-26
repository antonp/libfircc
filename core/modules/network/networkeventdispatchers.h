#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <eventdispatcher.h>
#include <networkmanagerevents.h>

namespace anp
{
	namespace firc
	{
		namespace network
		{
			typedef IEventDispatcherSubscriber<
				events::ISubscriber<events::NewNetwork>
			> NewNetworkEventDispatcher;
			
			typedef IEventDispatcherSubscriber<
				events::ISubscriber<events::RemovingNetwork>
			> RemovingNetworkEventDispatcher;
		}		
	}
}

#endif // _NETWORK_H_
