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

#ifndef _PLUGINMANAGER_H_
#define _PLUGINMANAGER_H_

#include <basedefs.h>
#include <networkeventdispatchers.h>

#include <string>

namespace anp
{
namespace irc
{
	class NetworkFactory;
	class Plugin;
	class PluginManagerImpl;

	/**
	 * Manages a list of loaded plugins.
	 */
	class PluginManager 
	{
	public:
		PluginManager();
		~PluginManager();

		/**
		 * Loads a plugin.
		 * 
		 * @param fileName
		 * Path to the shared library in which the plugin is implemented.
		 * 
		 * @param networkFactory
		 * The network factory exposed to this plugin. Can be used by the plugin
		 * to subscribe to events and open/close networks.
		 */
		void loadPlugin(
			const int8 *fileName,
			NetworkFactory &networkFactory,
			void *appContext
		);
		bool unloadPlugin(const std::string &fileName, uint32 reason);
		void unloadAllPlugins();
		uint32 getPluginCount() const;
		void getPluginInfo(uint32 index,
						   std::string &name);
	private:
		PluginManagerImpl *m_impl;
	};

} // namespace irc
} // namespace anp

#endif // _PLUGINMANAGER_H_
