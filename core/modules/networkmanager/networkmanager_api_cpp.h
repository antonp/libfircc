#ifndef _NETWORKMANAGER_API_CPP_H_
#define _NETWORKMANAGER_API_CPP_H_

#include <basedefs.h>

namespace anp
{
namespace firc
{
	class INetworkManagerFrontend;
	
	/**
	Creates a network manager and returns an INetworkManagerFrontend
	interface to it. Upon creation of the network manager, a
	connection is established to the specified host.

	@param host
	The name of the host to connect to. "irc.chatjunkies.org"
	for instance.

	@param port
	The port number to connect to. "6667" for instance.

	@param pluginManager
	The PluginManager to use for this network. Note that this will
	be refactored away from ths NetworkManager in a future update.

	@return
	An INetworkManagerFrontend interface to the network.
	*/
	INetworkManagerFrontend *
	networkmanager_create(const int8 *host, const int8 *port);

	/**
	Destroys the network manager. Disconnects the host if not
	already disconnected. Does not send any quit message to the
	host.

	@param networkManager
	The network manager to destroy.

	@see
	INetworkManagerFrontend::deinit
	*/
	void networkmanager_destroy(INetworkManagerFrontend *networkManager);
}
}

#endif // _NETWORKMANAGER_API_CPP_H_
