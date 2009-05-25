#ifndef _PLUGINMANAGER_H_
#define _PLUGINMANAGER_H_

#include <basedefs.h>
#include "inc/plugin_functions.h"

#include <vector> // johnny bigert this!

namespace firc
{
	class Plugin;

	class PluginManager
	{
	public:
		Result loadPlugins(uint32 count, const int8 *fileNames[]);
		Result loadPlugin(const int8 *fileName);
		Result unloadAllPlugins();
		
		void irc_onJoin(void *network, const int8 *channel,
						const int8 *user);
	private:
		enum
		{
			IRC_JOIN,
			IRC_PRIVMSG
		};
	
		std::vector<Plugin *>			m_plugins;
		std::vector<PF_irc_onJoin>		m_irc_onJoin_funcs;
	};

}

#endif // _PLUGINMANAGER_H_
