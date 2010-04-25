#ifndef _PLUGIN_FUNCTIONS_H_
#define _PLUGIN_FUNCTIONS_H_

#include <eventdispatcher.h>
#include <networkmanagerevents.h>
#include <networkeventdispatchers.h>

namespace anp
{
namespace firc
{
	// Abbreviations/prefixes:
	// pf - plugin function, implemented in plugins
	
	class INetworkManagerFrontend;
	class MsgPrefix;


	// Typedefs
	// uint32 pluginInit(void *fircCore)
	typedef uint32 (*PF_pluginInit)(		network::NewNetworkEventDispatcher &newNetworkDispatcher,		network::RemovingNetworkEventDispatcher &removingNetworkDispatcher,		void *appContext	);
	
	// void pluginDeinit()
	typedef void (*PF_pluginDeinit)(uint32);
} // namespace firc
} // namespace anp

#endif // _PLUGIN_FUNCTIONS_H_
