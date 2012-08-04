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

#ifndef _PLUGIN_H_
#define _PLUGIN_H_

#include <anpcode/anp_dynamic_library.h>
#include <fircc/networkeventdispatchers.h>

#include "plugin_functions.h"

#include <string>

namespace anp
{
namespace irc
{
    namespace networkfactory
    {
        class NetworkFactory;
    }

    /**
     * Represents a single plugin. Plugins can implement their own event
     * subscribers and add functionality to the client.
     */
    class Plugin
    {
    public:
        Plugin(
            const char *fileName,
            NetworkFactory &networkFactory,
            void *appContext
        );
        ~Plugin();

        void setUnloadReason(unsigned int reason);
        void setUnloading(bool unloading);
        bool isUnloading() const;

        const std::string &getName() const;
    private:
        DynamicLibrary      m_lib;

        PF_pluginDeinit     m_pf_pluginDeinit;

        // Info
        std::string         m_name;

        // Execution
        bool                m_unloading;
        unsigned int        m_unloadReason;

    };
} // namespace irc
} // namespace anp

#endif // _PLUGIN_H_
