#include <basedefs.h>
#include <network.h>

namespace anp
{
namespace firc
{
	class INetworkManagerFrontend;
	
	INetworkManagerFrontend *
	networkmanager_create(const int8 *host, const int8 *port)
	{
		return new Network(host, port);
	}
							
	void networkmanager_destroy(INetworkManagerFrontend *networkManager)
	{
		delete networkManager;
	}
}
}
