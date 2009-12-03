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
	m_fircCore(NULL),
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
	
	void PluginManager::setFircCore(void *fircCore)
	{
		m_fircCore = fircCore; // extend
	}
	
	/**
	 * @brief
	 * Load a set of plugins.
	 * 
	 * @param count
	 * Number of filenames
	 * 
	 * @param fileNames
	 * Array of NULL-terminated ascii
	 * strings representing the filenames.
	 */
	void PluginManager::loadPlugins(uint32 count,
									const int8 *fileNames[],
									bool32 acceptFailures)
	{
		if ( acceptFailures )
		{
			for ( uint32 i=0; i<count; ++i )
			{
				//std::cout << "loadPlugins: " << fileNames[i] << std::endl;
				try
				{
					loadPlugin(fileNames[i]);
				} catch ( std::runtime_error &e )
				{
					// Drop exception silently..?
				}
			}
		} else
		{
			for ( uint32 i=0; i<count; ++i )
			{
				try
				{
					loadPlugin(fileNames[i]);
				} catch ( ... )
				{
					unloadAllPlugins();
					throw;
				}
			}
		}
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

		Plugin *plugin = new Plugin(m_fircCore, fileName, &ioj, &iopm);
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
	 * 
	 * @return
	 * Returns CERR_OK on success or an error code on failure.
	 */
	Result PluginManager::unloadPluginReally(uint32 index, uint32 reason)
	{
		using std::vector;
		using std::pair;
		Result res = RES_OK;
		if ( index >= m_plugins.size() || NULL == m_plugins[index] )
		{
			return RES_INVALID_PARAMETER;
		}
		
		/// @todo Use reason parameter.
		m_plugins[index]->setUnloading(TRUE);
		return res;
	}
	
	Result PluginManager::getPluginCount(uint32 *count) const
	{
		if ( NULL != count )
		{
			*count = m_pluginCount;
			return RES_OK;
		} else
		{
			return RES_INVALID_PARAMETER;
		}
	}
	
	Result PluginManager::getPluginInfo(uint32 index,
							int8 *name, uint32 nameLength)
	{
		Result res = RES_INVALID_PARAMETER;
//		uint32 size = m_plugins.size();
		uint32 size = m_pluginCount;
		const std::string *n = NULL;
		
		if ( size > index && NULL != m_plugins[index] )
		{
			if ( NULL != name )
			{
				res = m_plugins[index]->getName(&n);
				if ( RES_OK == res )
				{
					const int8 *temp = n->c_str();
					uint32 tempSize = n->size();
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
					if ( i == tempSize )
					{
						res = RES_OK;
					} else
					{
						// The name didn't fit in the supplied buffer
						res = RES_BUFFER_TOO_SMALL;	
					}
				}
			}
		}
		return res;
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
	 * 
	 * @return
	 * Returns RES_OK on success or a specific error code on
	 * failure.
	 */
	Result PluginManager::addCallbackOnPrivMsg(PF_irc_onPrivMsg func)
	{
		Result res = RES_INVALID_PARAMETER;
		if ( NULL != func )
		{
			CallbackEntry entry = { (void *)func, NULL };
			m_callbacks[IRC_PRIVMSG].push_back(entry);
			res = RES_OK;
		}
		return res;
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
	
	/* Native functions */
	extern "C"
	{
		/**
		 * @brief
		 * Load a plugin.
		 * 
		 * @param pluginManager
		 * Valid pointer to a (the) plugin manager object.
		 * 
		 * @param fileName
		 * Filename of the plugin to load.
		 * 
		 * @return
		 * RES_OK on success or a specific error code on failure.
		 */
		Result pluginLoad(void *pluginManager, const int8 *fileName)
		{
			Result res = RES_INVALID_PARAMETER;
			
			if ( NULL != pluginManager && NULL != fileName )
			{
				res = ((PluginManager *)pluginManager)->loadPlugin(
															fileName);
			}
			return res;
		}
		
		/**
		 * @brief
		 * Get the number of loaded plugins.
		 * 
		 * @param pluginManager
		 * Valid pointer to a (the) plugin manager object.
		 * 
		 * @param[out] count
		 * Valid pointer to an unsigned integer to receive the number.
		 * 
		 * @return
		 * Returns RES_OK on success or a specific error code on
		 * failure.
		 */
		Result pluginGetPluginCount(void *pluginManager, uint32 *count)
		{
			Result res = RES_INVALID_PARAMETER;
			
			if ( NULL != pluginManager && NULL != count )
			{
				res = ((PluginManager *)pluginManager)
							->getPluginCount(count);
			}
			return res;
		}

		/**
		 * @brief
		 * Retrieve information about a loaded plugin.
		 * 
		 * @param pluginManager
		 * Pointer to a plugin manager.
		 * 
		 * @param index
		 * The location in the plugin manager's list.
		 * 
		 * @param[out] name
		 * Pointer to a buffer which will receive the name.
		 * 
		 * @param nameLength
		 * Maximum number of characters that the name buffer can hold.
		 * 
		 * @return
		 * RES_OK on success or a specific error code on failure.
		 */		
		Result pluginGetPluginInfo(void *pluginManager, uint32 index,
									int8 *name, uint32 nameLength)
		{
			Result res = RES_INVALID_PARAMETER;
			
			if ( NULL != pluginManager )
			{
				res = ((PluginManager *)pluginManager)->getPluginInfo(
					index,
					name, nameLength);
			}
			return res;
		}
		
		/**
		 * @brief 
		 * Schedule a specific plugin for unloading.
		 * 
		 * @param pluginManager
		 * Pointer to a plugin manager.
		 * 
		 * @param index
		 * The location in the plugin manager's internal list.
		 * 
		 * @param reason
		 * Reason for unload. (Will introduce some predefined constants
		 * for this parameter later, like RELOAD/UPGRADE/SHUTDOWN etc).
		 */
		Result pluginUnload(void *pluginManager, uint32 index,
							uint32 reason)
		{
			Result res = RES_INVALID_PARAMETER;
			
			if ( NULL != pluginManager )
			{
				res = ((PluginManager *)pluginManager)->
					unloadPluginReally(index, reason);
			}
			
			return res;
		}
	}
} // namespace firc
} // namespace anp
