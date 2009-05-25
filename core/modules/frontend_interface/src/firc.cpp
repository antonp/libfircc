#include "../firc.h"
#include <networkmanager.h>
#include <iostream> /// @todo remove

namespace firc
{
	Result Core::init()
	{
		Result res = RES_FAILED;
		const int8 *pluginNames[] = {
			"./libpluginTest1.so"
		};
		
		res = m_pluginManager.loadPlugins(sizeof(pluginNames)/sizeof(int8 *),
									pluginNames);
		if ( RES_OK != res )
		{
			std::cout << "Failed to load plugins." << std::endl;
			return res;
		}

		NetworkManager *net1 = new NetworkManager;
		if ( NULL == net1 )
		{
			return RES_MEMALLOC_FAILED;
		}
		res = net1->init();
		if ( RES_OK != res )
		{
			delete net1;
			return res;
		}
		m_networkManagers.push_back(net1);
		
		return RES_OK;
	}
	
	bool32 Core::update()
	{
		return true;
	}
}
