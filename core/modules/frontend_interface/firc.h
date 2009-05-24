#ifndef _FIRC_H_
#define _FIRC_H_

#include <basedefs.h>
#include <pluginmanager.h> // johnny bigert this!

#include <vector> // johnny bigert this!

namespace firc
{
	class NetworkManager;
	
	class Core
	{
	public:
		Result init();
		bool32 update();
	private:
		PluginManager m_pluginManager;
		std::vector<NetworkManager *> m_networkManagers;
	};
}

#endif // _FIRC_H_
