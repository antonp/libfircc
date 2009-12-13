#include <basedefs.h>
#include <networkmanager.h>

namespace anp
{
namespace firc
{
	class INetworkManagerFrontend;
	
	INetworkManagerFrontend *
	networkmanager_create(const int8 *host, const int8 *port,
							PluginManager *pluginManager)
	{
		return new NetworkManager(host, port, pluginManager);
	}
							
	void networkmanager_destroy(INetworkManagerFrontend *networkManager)
	{
		delete networkManager;
	}
}
}
