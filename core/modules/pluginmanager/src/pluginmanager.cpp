#include "../pluginmanager.h"
#include "../inc/plugin.h"

#include <iostream>
#include <algorithm>
#include <stdexcept>

namespace anp
{
namespace firc
{
	PluginManager::PluginManager()
	{
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
	void PluginManager::loadPlugin(
		const int8 *fileName,
		network::NewNetworkEventDispatcher &newNetworkDispatcher,
		network::RemovingNetworkEventDispatcher &removingNetworkDispatcher,
		void *appContext
	)
	{
		Plugin *plugin = new Plugin(
			fileName,
			newNetworkDispatcher,
			removingNetworkDispatcher,
			appContext
		);
		m_plugins.push_back(plugin);
	}

	/**
		@brief
		Unloads plugin identified by fileName.

		@param fileName
		Filename of the plugin to unload.

		@param reason
		Reason for the unload.
	*/
	bool PluginManager::unloadPlugin(const std::string &fileName,
									 uint32 reason)
	{
		for ( uint32 i=0; i<m_plugins.size(); ++i )
		{
			if ( m_plugins[i]->getName() == fileName )
			{
				m_plugins[i]->setUnloadReason(reason);
				delete m_plugins[i];
				m_plugins.erase(m_plugins.begin()+i);
				return true;
			}
		}
		return false;
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
	}
	
	uint32 PluginManager::getPluginCount() const
	{
		return m_plugins.size();
	}
	
	void PluginManager::getPluginInfo(uint32 index,
									  std::string &name)
	{
		uint32 size = m_plugins.size();
		
		if ( size > index && NULL != m_plugins[index] )
		{
			name = m_plugins[index]->getName();
		} else
		{
			throw std::runtime_error("getPluginInfo() index out of bounds");
		}
	}
} // namespace firc
} // namespace anp
