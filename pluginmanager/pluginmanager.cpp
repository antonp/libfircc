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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <fircc/pluginmanager.h>
#include "inc/plugin.h"

#include <vector>
#include <algorithm>
#include <stdexcept>

namespace anp
{
namespace irc
{
	class PluginManagerImpl
	{
	public:
		
		std::vector<Plugin *> m_plugins;
	};
	
	PluginManager::PluginManager():
	m_impl(new PluginManagerImpl)
	{
	}
	
	PluginManager::~PluginManager()
	{
		unloadAllPlugins();
		delete m_impl;
	}
	
	/**
	 * @brief
	 * Load a single plugin.
	 * 
	 * @param fileName
	 * NULL terminated string representing the fileName
	 */
	void PluginManager::loadPlugin(
		const char *fileName,
		NetworkFactory &networkFactory,
		void *appContext
	)
	{
		Plugin *plugin = new Plugin(
			fileName,
			networkFactory,
			appContext
		);
		m_impl->m_plugins.push_back(plugin);
	}

	bool PluginManager::unloadPlugin(const std::string &fileName,
									 unsigned int reason)
	{
		std::vector<Plugin *> &plugins = m_impl->m_plugins;
		for ( unsigned int i=0; i<plugins.size(); ++i )
		{
			if ( plugins[i]->getName() == fileName )
			{
				plugins[i]->setUnloadReason(reason);
				delete plugins[i];
				plugins.erase(plugins.begin()+i);
				return true;
			}
		}
		return false;
	}
	
	void PluginManager::unloadAllPlugins()
	{
		std::vector<Plugin *> &plugins = m_impl->m_plugins;
		unsigned int size = plugins.size();
		Plugin *p = NULL;
		for ( unsigned int i=0; i<size; ++i )
		{
			p = plugins[i];
			if ( NULL == p )
			{
				// weird, but ok skip this one then
				continue;
			} else
			{
				p->setUnloadReason(0); // Revise '0'
				delete p;
				p = NULL;
				plugins[i] = NULL;
			}
		}
		plugins.clear();
	}
	
	unsigned int PluginManager::getPluginCount() const
	{
		return m_impl->m_plugins.size();
	}
	
	void PluginManager::getPluginInfo(unsigned int index,
									  std::string &name)
	{
		std::vector<Plugin *> &plugins = m_impl->m_plugins;
		unsigned int size = plugins.size();
		
		if ( size > index && NULL != plugins[index] )
		{
			name = plugins[index]->getName();
		} else
		{
			throw std::invalid_argument("getPluginInfo() index out of bounds");
		}
	}
} // namespace irc
} // namespace anp
