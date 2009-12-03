#include "../core.h"
#include <networkmanager.h>
#include <anp_exceptions.h>

namespace anp
{
namespace firc
{
	Core::Core(uint8 pluginCount, const int8 *pluginNames[]):
	ICoreFrontend()
	{
		m_pluginManager.setFircCore((void *)this);
		m_pluginManager.loadPlugins(pluginCount,
									pluginNames);
	}
	
	Core::~Core()
	{
		for ( uint32 i=0; i<m_networkManagers.size(); ++i )
		{
			delete m_networkManagers[i];
		}
		m_networkManagers.clear();
	}
	
	INetworkManagerFrontend *Core::createNetworkManager(const int8 *host,
												const int8 *port)
	{
		NetworkManager *nm(new NetworkManager(host, port,
							&m_pluginManager));
		m_networkManagers.push_back(nm);
		return nm;
	}
	
	void Core::destroyNetworkManager(
								INetworkManagerFrontend *networkManager,
								const int8 *message)
	{
		Result res = RES_INVALID_PARAMETER;

		if ( NULL != networkManager )
		{
			for ( uint32 i=0; i<m_networkManagers.size(); ++i )
			{
				if ( m_networkManagers[i] == networkManager )
				{
					m_networkManagers[i]->deinit(message);
					
					delete networkManager;
					m_networkManagers.erase(
										m_networkManagers.begin()+i);
					return;
				}
			}
		}
	}
	
	PluginManager *Core::getPluginManager()
	{
		return &m_pluginManager;
	}
	
	void Core::addCallbackOnPrivMsg(PF_irc_onPrivMsg func)
	{
		if ( NULL != func )
		{
			Result res = m_pluginManager.addCallbackOnPrivMsg(func);
			if ( res != RES_OK )
			{
				throw std::runtime_error("m_pluginManager.addCallbackOnPrivMsg(..) != RES_OK");
			}
		} else
		{
			throw std::invalid_argument("func == NULL");
		}
	}
} // namespace firc
} // namespace anp
