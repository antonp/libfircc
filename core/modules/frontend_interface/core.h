#ifndef _CORE_H_
#define _CORE_H_

#include <basedefs.h>
#include <pluginmanager.h> // johnny bigert this!
#include "core_frontend.h"

#include <vector> // johnny bigert this!
#include <memory>

namespace anp
{
namespace firc
{
	class INetworkManagerFrontend;
	class NetworkManager;
	
	class Core: public ICoreFrontend
	{
	public:
		Core(uint8 pluginCount, const int8 *pluginNames[]);
		virtual ~Core();
		
		INetworkManagerFrontend *createNetworkManager(const int8 *host,
											 const int8 *port);
									
		void destroyNetworkManager(INetworkManagerFrontend *networkManager,
										const int8 *message);
										
		PluginManager *getPluginManager();
		
		void addCallbackOnPrivMsg(PF_irc_onPrivMsg func);
	private:
		PluginManager m_pluginManager;
		std::vector<NetworkManager *> m_networkManagers;
	};
} // namespace firc
} // namespace anp

#endif // _CORE_H_
