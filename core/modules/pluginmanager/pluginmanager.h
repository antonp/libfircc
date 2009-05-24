#ifndef _PLUGINMANAGER_H_
#define _PLUGINMANAGER_H_

#include <basedefs.h>

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
		
		
	private:
		std::vector<Plugin *> m_plugins;
	};

}

#endif // _PLUGINMANAGER_H_
