#ifndef _FIRC_H_
#define _FIRC_H_

#include <basedefs.h>
#include <pluginmanager.h> // johnny bigert this!

#include <vector> // johnny bigert this!

namespace anp
{
namespace firc
{
	class NetworkManager;
	
	class Core
	{
	public:
		Result init(uint8 pluginCount, const int8 *pluginNames[]);
		bool32 update();
		
		Result createNetworkManager(const int8 *host, const int8 *port,
									NetworkManager **networkManager);
									
		Result destroyNetworkManager(NetworkManager *networkManager,
										const int8 *message);
										
		Result getPluginManager(PluginManager **pluginManager);
		
		Result addCallbackOnPrivMsg(PF_irc_onPrivMsg func);
	private:
		PluginManager m_pluginManager;
		std::vector<NetworkManager *> m_networkManagers;
	};
} // namespace firc
} // namespace anp

#endif // _FIRC_H_
