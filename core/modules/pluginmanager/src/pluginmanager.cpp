#include "../pluginmanager.h"
#include "../inc/plugin.h"

#include <iostream>
#include <algorithm>
#include <stdexcept>

namespace anp
{
namespace firc
{
	PluginManager::PluginManager():
	m_pluginCount(0)
	{
		for ( uint32 i=0; i<IRC_MAX_CALLBACKS; ++i )
		{
			m_callbacks[i].reserve(10);
		}
	}
	
	PluginManager::~PluginManager()
	{
		unloadAllPlugins();
	}
	
	/**
	 * @brief
	 * Load a single plugin.
	 * 
	 * @param fileName
	 * NULL terminated string representing the fileName
	 */
	void PluginManager::loadPlugin(const int8 *fileName)
	{
		Result res = RES_FAILED;
		CallbackEntry entry = { NULL, NULL };

		PF_irc_onJoin ioj = NULL;
		PF_irc_onPrivMsg iopm = NULL;

		Plugin *plugin = new Plugin(NULL, fileName, &ioj, &iopm);
		m_plugins.push_back(plugin);
		
		entry.plugin = plugin;
		
		if ( NULL != ioj )
		{
			entry.func = (void *)ioj;
			m_callbacks[IRC_JOIN].push_back(entry);
		}
		if ( NULL != iopm )
		{
			entry.func = (void *)iopm;
			m_callbacks[IRC_PRIVMSG].push_back(entry);
		}
		++m_pluginCount;
	}
	
	/**
	 * @brief
	 * Unloads all currently loaded plugins.
	 */
	void PluginManager::unloadAllPlugins()
	{
		uint32 size = m_plugins.size();
		Plugin *p = NULL;
		for ( uint32 i=0; i<size; ++i )
		{
			p = m_plugins[i];
			if ( NULL == p )
			{
				// weird, but ok skip this one then
				continue;
			} else
			{
				p->setUnloadReason(0); // Revise '0'
				delete p;
				p = NULL;
				m_plugins[i] = NULL;
			}
		}
		m_plugins.clear();
		m_pluginCount = 0;
		
		m_pluginsToBeUnloaded.clear();
	}
	
	/**
	 * @brief
	 * Unloads a specific plugin.
	 * 
	 * @param index
	 * The location in the plugin manager's internal list.
	 * 
	 * @param reason
	 * Reason for unload. (Will introduce some predefined constants
	 * for this parameter later, like RELOAD/UPGRADE/SHUTDOWN etc).
	 */
	void PluginManager::unloadPluginReally(uint32 index, uint32 reason)
	{
		using std::vector;
		using std::pair;

		if ( index >= m_plugins.size() || NULL == m_plugins[index] )
		{
			throw std::invalid_argument("index out-of-bounds");
		}
		
		/// @todo Use reason parameter.
		m_plugins[index]->setUnloading(TRUE);
	}
	
	uint32 PluginManager::getPluginCount() const
	{
		return m_pluginCount;
	}
	
	void PluginManager::getPluginInfo(uint32 index,
							int8 *name, uint32 nameLength)
	{
//		uint32 size = m_plugins.size();
		uint32 size = m_pluginCount;
		
		if ( size > index && NULL != m_plugins[index] )
		{
			if ( NULL != name )
			{
				const std::string &n = m_plugins[index]->getName();
				const int8 *temp = n.c_str();
				uint32 tempSize = n.size();
				uint32 i=0;
				for ( ; i<nameLength && i<tempSize; ++i )
				{
					name[i] = temp[i];
				}
				if ( i < nameLength )
				{
					name[i] = 0; // Null-terminate
				} else
				{
					name[nameLength-1] = 0; // Null-terminate
				}
				if ( i != tempSize )
				{
					// The name didn't fit in the supplied buffer
					throw std::invalid_argument("buffer too small");
				}
			}
		}
	}

	/**
	 * @brief
	 * Calls every loaded plugin's _type_ callback function.
	 */	
	void PluginManager::performJob(PluginJob *job, CallbackType type)
	{
		using std::vector;
		using std::pair;
		
		vector<CallbackEntry> &vec = m_callbacks[type];
		
		for ( vector<CallbackEntry>::iterator i =
				vec.begin();
				i != vec.end(); )
		{
			CallbackEntry &entry = (*i);
			Plugin *plugin = entry.plugin;
			
			if ( NULL != plugin )
			{
				if ( plugin->isUnloading() )
				{
					addPluginToUnloadList(plugin);
					i++;
					continue;
				}
				job->setPlugin(plugin);
				plugin->increaseExecutionCount();
			}
			job->setFunc(entry.func);
			job->execute();
			if ( NULL != plugin )
			{
				plugin->decreaseExecutionCount();
			}
			
			i++;
		}
		
		unloadScheduledPlugins();
	}

	/**
	 * @brief
	 * Adds a function to be called whenever a PRIVMSG is received
	 * from the IRC network.
	 * 
	 * @param func
	 * The function to call when a PRIVMSG is received.
	 */
	void PluginManager::addCallbackOnPrivMsg(PF_irc_onPrivMsg func)
	{
		if ( NULL != func )
		{
			CallbackEntry entry = { (void *)func, NULL };
			m_callbacks[IRC_PRIVMSG].push_back(entry);
		} else
		{
			throw std::invalid_argument("func == NULL");
		}
	}
	
	void PluginManager::addPluginToUnloadList(Plugin *const plugin)
	{	
		// Check if it's already in the list	
		for ( uint32 i=0; i<m_pluginsToBeUnloaded.size(); ++i )
		{
			if ( plugin == m_pluginsToBeUnloaded[i] )
			{
				return;
			}
		}
		m_pluginsToBeUnloaded.push_back(plugin);
	}
	
	void PluginManager::unloadScheduledPlugins()
	{
		using std::vector;
		using std::pair;
		
		// Remove all callback entries associated with the plguins
		// scheduled for unload.
		for ( uint32 i=0; i<m_pluginsToBeUnloaded.size(); ++i )
		{
			for ( uint32 k=0; k<IRC_MAX_CALLBACKS; ++k )
			{
				vector<CallbackEntry> &vec = m_callbacks[k];
				for ( vector<CallbackEntry>::iterator j = vec.begin();
					j != vec.end(); )
				{
					if ( m_pluginsToBeUnloaded[i] == (*j).plugin )
					{
						j = vec.erase(j);
					} else
					{
						j++;
					}
				}
			}
			
			// Remove the plugin reference from the list of plugins
			for ( vector<Plugin *>::iterator l=m_plugins.begin();
				l!=m_plugins.end(); )
			{
				if ( m_pluginsToBeUnloaded[i] == (*l) )
				{
					l = m_plugins.erase(l);
				} else
				{
					l++;
				}
			}
			
			// Free up the plugin resources
			delete m_pluginsToBeUnloaded[i];
			m_pluginCount--;
		}
		m_pluginsToBeUnloaded.clear();
	}
} // namespace firc
} // namespace anp
