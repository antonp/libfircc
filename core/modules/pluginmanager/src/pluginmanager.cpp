/*

Copyright (c) 2010, Anton Petersson < anton (at) anp.nu >
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "../pluginmanager.h"
#include "../inc/plugin.h"

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
		NetworkFactory &networkFactory,
		void *appContext
	)
	{
		Plugin *plugin = new Plugin(
			fileName,
			networkFactory,
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
			throw std::invalid_argument("getPluginInfo() index out of bounds");
		}
	}
} // namespace firc
} // namespace anp
