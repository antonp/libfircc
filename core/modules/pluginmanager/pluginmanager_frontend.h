#ifndef _PLUGINMANAGER_FRONTEND_H_
#define _PLUGINMANAGER_FRONTEND_H_

#include <basedefs.h>
#include "plugin_jobs.h"

namespace anp
{
namespace firc
{
	class Plugin;
	
	class IPluginManagerFrontend
	{
	public:
		IPluginManagerFrontend() { };
		virtual ~IPluginManagerFrontend() { };

		enum CallbackType
		{
			IRC_JOIN,
			IRC_PART,
			IRC_PRIVMSG,
			IRC_TOPIC,
			
			IRC_MAX_CALLBACKS
		};

		virtual void loadPlugin(const int8 *fileName) = 0;
		virtual void unloadAllPlugins() = 0;
		virtual uint32 getPluginCount() const = 0;
		virtual void getPluginInfo(uint32 index,
							std::string &name) = 0;
	};
}
}

#endif // _PLUGINMANAGER_FRONTEND_H_
