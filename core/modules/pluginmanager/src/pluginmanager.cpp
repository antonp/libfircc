#include "../pluginmanager.h"
#include "../inc/plugin.h"

#include <iostream>

namespace firc
{
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
		Plugin *plugin = new Plugin;
		
		if ( NULL == plugin )
		{
			return RES_MEMALLOC_FAILED;
		}
		res = plugin->loadFromFile(fileName);
		if ( RES_OK != res )
		{
			delete plugin;
			plugin = NULL;
			return res;
		}
		m_plugins.push_back(plugin);

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
				// weird
				continue;
			} else
			{
				p->unload();
				delete p;
				p = NULL;
				m_plugins[i] = NULL;
			}
		}
		m_plugins.clear();
	}
}
