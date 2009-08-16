#ifndef _PLUGINMANAGER_H_
#define _PLUGINMANAGER_H_

#include <basedefs.h>
#include <plugin_functions.h>

#include <vector> // johnny bigert this!
#include <map>

namespace anp
{
namespace firc
{
	class Plugin;

	class PluginManager
	{
	public:
		PluginManager();

		void setFircCore(void *fircCore); // Extend
	
		Result loadPlugins(uint32 count, const int8 *fileNames[]);
		Result loadPlugin(const int8 *fileName);
		Result unloadAllPlugins();
		Result unloadPlugin(uint32 index, uint32 reason);
		Result getPluginCount(uint32 *count) const;
		Result getPluginInfo(uint32 index,
							int8 *name, uint32 nameLength);
		
		void irc_onJoin(void *network, const int8 *channel,
						const int8 *user);
		void irc_onPrivMsg(void *network, const int8 *sender,
							const int8 *target,
							const int8 *message);
							
		Result addCallbackOnPrivMsg(PF_irc_onPrivMsg func);
	private:
		enum
		{
			IRC_JOIN,
			IRC_PRIVMSG
		};
	
		void							*m_fircCore; // Extend
		uint32							m_pluginCount;
		std::vector<Plugin *>			m_plugins;
		
		std::vector<std::pair<PF_irc_onJoin, Plugin *> >
										m_irc_onJoin_funcs;
		std::vector<std::pair<PF_irc_onPrivMsg, Plugin *> >
										m_irc_onPrivMsg_funcs;
	};

} // namespace firc
} // namespace anp

#endif // _PLUGINMANAGER_H_
