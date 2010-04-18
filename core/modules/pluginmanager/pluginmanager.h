#ifndef _PLUGINMANAGER_H_
#define _PLUGINMANAGER_H_

#include <basedefs.h>
#include <plugin_functions.h>
#include <anp_jobqueue.h>
#include "pluginmanager_frontend.h"

// TODO: Maybe pImpl this
#include <vector> 
#include <map>
#include <string>
#include "plugin_jobs.h" // remove this dependency by moving the stuff in this header to another

namespace anp
{
namespace firc
{
	class Plugin;

	class PluginManager 
	{
	public:
		PluginManager();
		~PluginManager();

		void loadPlugin(const int8 *fileName);
		bool unloadPlugin(const std::string &fileName, uint32 reason);
		void unloadAllPlugins();
		uint32 getPluginCount() const;
		void getPluginInfo(uint32 index,
						   std::string &name);
	private:
		std::vector<Plugin *>			m_plugins;
	};

} // namespace firc
} // namespace anp

#endif // _PLUGINMANAGER_H_
