#include "../pluginmanager.h"
#include "../inc/plugin.h"

#include <iostream>
#include <algorithm>

namespace anp
{
namespace firc
{
	PluginManager::PluginManager():
	m_fircCore(NULL),
	m_pluginCount(0)
	{
		
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
	 * 
	 * @return
	 * Returns RES_OK on success or a specific error code on failure.
	 */
	Result PluginManager::loadPlugins(uint32 count,
										const int8 *fileNames[])
	{
		Result res;
		for ( uint32 i=0; i<count; ++i )
		{
			std::cout << "loadPlugins: " << fileNames[i] << std::endl;
			res = loadPlugin(fileNames[i]);
			if ( RES_OK != res )
			{
				unloadAllPlugins();
				return res;
			}
		}
		return RES_OK;
	}
	
	/**
	 * @brief
	 * Load a single plugin.
	 * 
	 * @param fileName
	 * NULL terminated string representing the fileName
	 * 
	 * @return
	 * Returns RES_OK on success or a specific error code on failure.
	 */
	Result PluginManager::loadPlugin(const int8 *fileName)
	{
		Result res = RES_FAILED;

		PF_irc_onJoin ioj = NULL;
		PF_irc_onPrivMsg iopm = NULL;

		Plugin *plugin = new Plugin;
		
		if ( NULL == plugin )
		{
			return RES_MEMALLOC_FAILED;
		}
		res = plugin->loadFromFile(m_fircCore, fileName, &ioj, &iopm);
		if ( RES_OK != res )
		{
			delete plugin;
			plugin = NULL;
			return res;
		}
		m_plugins.push_back(plugin);
		
		if ( NULL != ioj )
		{
			m_irc_onJoin_funcs.push_back(
				std::pair<PF_irc_onJoin, Plugin *>(ioj, plugin));
		}
		if ( NULL != iopm )
		{
			m_irc_onPrivMsg_funcs.push_back(
				std::pair<PF_irc_onPrivMsg, Plugin *>(iopm, plugin));
		}
		++m_pluginCount;

		return RES_OK;
	}
	
	/**
	 * @brief
	 * Unloads all currently loaded plugins.
	 * 
	 * @return
	 * Returns RES_OK on success or a specific error code on failure.
	 */
	Result PluginManager::unloadAllPlugins()
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
				p->unload(0); // Revise '0'
				delete p;
				p = NULL;
				m_plugins[i] = NULL;
			}
		}
		m_plugins.clear();
		m_pluginCount = 0;
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
		res = m_plugins[index]->setUnloading(TRUE);
		
/*		// Plugin list and memory cleanup
		delete m_plugins[index];
		m_plugins[index] = NULL; // List becomes fragmented :\
		// assert m_pluginCount > 0 (todo)
		--m_pluginCount;
		
		std::vector<Plugin *>::iterator i = m_plugins.begin()+index;
		m_plugins.erase(i); // Not fragmented anymore
		*/
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
		uint32 size = m_plugins.size();
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
	 * Calls every loaded plugin's irc_onJoin function.
	 * 
	 * @param network
	 * The network that sent the JOIN message.
	 * 
	 * @param channel
	 * The channel that someone joined.
	 * 
	 * @param user
	 * The nickname of the user who joined the channel.
	 */	
	void PluginManager::irc_onJoin(void *network, const int8 *channel,
									const int8 *user)
	{
		using std::vector;
		using std::pair;
		uint32 funcCount = m_irc_onJoin_funcs.size();
		uint32 i = 0;
		pair<PF_irc_onJoin, Plugin *> *entry;
		Plugin *plugin = NULL;
		for ( ; i<funcCount; ++i )
		{
			// Call the onJoin funcs
			entry = &m_irc_onJoin_funcs[i];
			
			plugin = entry->second;
			if ( NULL != plugin )
			{
				if ( plugin->isUnloading() )
				{
					continue;
				}
				plugin->increaseExecutionCount();
			}
			entry->first(network, channel, user);
			if ( NULL != plugin )
			{
				plugin->decreaseExecutionCount();
			}
			
			/** @todo Later, add a task with function (first) and
			Plugin * (second). */
		}
		
		// Remove all callbacks associated with plugins being unloaded.
		/// @todo Improve this by doing it directly in the first loop
		/// in this function. (This is unnessecarily slow)
		for ( vector<pair<PF_irc_onJoin, Plugin *> >::iterator i =
				m_irc_onJoin_funcs.begin();
				i != m_irc_onJoin_funcs.end(); )
		{
			plugin = (*i).second;
			if ( NULL != plugin && plugin->isUnloading() )
			{
				///@todo Call the real unload function here since this
				/// is only called from the messagereceiverthread.
				plugin->unload(0);
				i = m_irc_onJoin_funcs.erase(i);
			} else
			{
				++i;
			}
		}
	}
	
	/**
	 * @brief
	 * Calls every loaded plugin's irc_onPrivMsg function.
	 * 
	 * @param network
	 * The network that sent the PRIVMSG message.
	 * 
	 * @param sender
	 * The sender of the message.
	 * 
	 * @param target
	 * The receiver of the message, channel or user.
	 * 
	 * @param message
	 * The message.
	 */
	void PluginManager::irc_onPrivMsg(void *network, const int8 *sender,
										const int8 *target,
										const int8 *message)
	{
//		uint32 funcCount = m_irc_onPrivMsg_funcs.size();
		uint32 i = 0;
		std::pair<PF_irc_onPrivMsg, Plugin *> *entry;
//		for ( ; i<funcCount; ++i )
		for ( ; i<m_irc_onPrivMsg_funcs.size(); ++i )
		{
			// Call the onPrivMsg funcs
			entry = &m_irc_onPrivMsg_funcs[i];
			
			if ( NULL != entry->second )
			{
				entry->second->increaseExecutionCount();
			}
			entry->first(network, sender, target, message);
			if ( NULL != entry->second )
			{
				entry->second->decreaseExecutionCount();
			}

			/** @todo Later, add a task with function (first) and
			Plugin * (second). */
		}
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
			m_irc_onPrivMsg_funcs.push_back(
				std::pair<PF_irc_onPrivMsg, Plugin *>(func, NULL));
			res = RES_OK;
		}
		return res;
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
