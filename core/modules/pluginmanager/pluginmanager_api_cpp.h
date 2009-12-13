#ifndef _PLUGINMANAGER_API_CPP_H_
#define _PLUGINMANAGER_API_CPP_H_

namespace anp
{
namespace firc
{
	class IPluginManagerFrontend;
	
	IPluginManagerFrontend *pluginmanager_create();
	void pluginmanager_destroy(IPluginManagerFrontend *pluginManager);
}
}

#endif // _PLUGINMANAGER_API_CPP_H_
