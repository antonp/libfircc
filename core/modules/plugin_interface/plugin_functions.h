#ifndef _PLUGIN_FUNCTIONS_H_
#define _PLUGIN_FUNCTIONS_H_

#include <eventdispatcher.h>
#include <networkevents.h>
#include <networkeventdispatchers.h>

namespace anp
{
namespace firc
{
	class NetworkFactory;
	
	// Abbreviations/prefixes:
	// pf - plugin function, implemented in plugins
	
	class INetworkManagerFrontend;
	class MsgPrefix;


	// Typedefs
	// uint32 pluginInit(void *fircCore)
	typedef uint32 (*PF_pluginInit)(NetworkFactory &, void *appContext);
	
	// void pluginDeinit()
	typedef void (*PF_pluginDeinit)(uint32);
} // namespace firc
} // namespace anp

#endif // _PLUGIN_FUNCTIONS_H_
