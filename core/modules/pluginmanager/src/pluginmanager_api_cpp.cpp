#include <basedefs.h>
#include "../pluginmanager.h"

namespace anp
{
namespace firc
{
	class IPluginManagerFrontend;
	
	IPluginManagerFrontend *pluginmanager_create()
	{
		return new PluginManager;
	}
	
	void pluginmanager_destroy(IPluginManagerFrontend *pluginManager)
	{
		delete pluginManager;
	}
}
}
