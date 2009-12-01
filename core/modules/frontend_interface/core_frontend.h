#ifndef _CORE_FRONTEND_H_
#define _CORE_FRONTEND_H_

#include <basedefs.h>

namespace anp
{
namespace firc
{
	class NetworkManager;
	class PluginManager;
	
	class ICoreFrontend
	{
	public:
		ICoreFrontend();
		virtual ~ICoreFrontend();
	
		virtual NetworkManager *createNetworkManager(
												const int8 *host,
											 	const int8 *port) = 0;
		virtual void destroyNetworkManager(
										NetworkManager *networkManager,
										const int8 *message) = 0;
										
		virtual PluginManager *getPluginManager();
		
		virtual void addCallbackOnPrivMsg(PF_irc_onPrivMsg func);
	};
}
}

#endif // _CORE_FRONTEND_H_
