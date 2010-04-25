#ifndef _PLUGINMANAGER_H_
#define _PLUGINMANAGER_H_

#include <basedefs.h>
#include <networkeventdispatchers.h>

// TODO: Maybe pImpl this
#include <vector> 
#include <string>

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

		void loadPlugin(
			const int8 *fileName,
			network::NewNetworkEventDispatcher &newNetworkDispatcher,
			network::RemovingNetworkEventDispatcher &removingNetworkDispatcher,
			void *appContext
		);
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
