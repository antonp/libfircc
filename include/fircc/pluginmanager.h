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

#include <fircc/networkeventdispatchers.h>

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
         *
         * @param appContext
         * Contextual data specific to the application using the lib.
         * This is not used by the lib.
         * The idea is to allow app specific plugins to be written.
         * For instance, the application may expose a gui api to the plugin.
         */
        void loadPlugin(
            const char *fileName,
            NetworkFactory &networkFactory,
            void *appContext
        );

        /**
         * Unloads plugin identified by fileName.
         *
         * @param fileName
         * Filename of the plugin to unload.
         *
         * @param reason
         * Reason for the unload. Currently undefined.
         */
        bool unloadPlugin(const std::string &fileName, unsigned int reason);

        /**
         * Unloads all currently loaded plugins.
         */
        void unloadAllPlugins();
        unsigned int getPluginCount() const;

        /**
         * Retrieves information about a loaded plugin.
         *
         * @param index
         * Index in the list of plugins.
         *
         * @param [out] name
         * Name of the plugin at the specified index.
         *
         * @remark
         * The list of plugins may have been modified from another thread
         * since getPluginCount() was called. This may be a problem, depending
         * on the application. In that case, be ready to catch
         * std::invalid_argument exceptions.
         */
        void getPluginInfo(unsigned int index,
                           std::string &name);
    private:
        PluginManagerImpl *m_impl;
    };

} // namespace irc
} // namespace anp

#endif // _PLUGINMANAGER_H_
