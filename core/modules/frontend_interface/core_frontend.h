#ifndef _CORE_FRONTEND_H_
#define _CORE_FRONTEND_H_

#include <basedefs.h>
#include <plugin_functions.h>

namespace anp
{
namespace firc
{
	class INetworkManagerFrontend;
	class NetworkManager;
	class PluginManager;
	
	class ICoreFrontend
	{
	public:
		ICoreFrontend() { }
		virtual ~ICoreFrontend() { }
	
		virtual INetworkManagerFrontend *createNetworkManager(
												const int8 *host,
											 	const int8 *port) = 0;
		virtual void destroyNetworkManager(
								INetworkManagerFrontend *networkManager,
								const int8 *message) = 0;
										
		virtual PluginManager *getPluginManager() = 0;
		
		virtual void addCallbackOnPrivMsg(PF_irc_onPrivMsg func) = 0;
	};
}
}

#endif // _CORE_FRONTEND_H_
