#ifndef _NETWORKMANAGER_API_CPP_H_
#define _NETWORKMANAGER_API_CPP_H_

#include <basedefs.h>

namespace anp
{
namespace firc
{
	class INetworkManagerFrontend;
	class IPluginManagerFrontend;
	
	INetworkManagerFrontend *
	networkmanager_create(const int8 *host, const int8 *port,
							PluginManager *pluginManager);
							
	void networkmanager_destroy(INetworkManagerFrontend *networkManager);
}
}

#endif // _NETWORKMANAGER_API_CPP_H_
