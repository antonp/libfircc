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

#ifndef _PLUGIN_FUNCTIONS_H_
#define _PLUGIN_FUNCTIONS_H_

#include <anpcode/eventdispatcher.h>
#include <fircc/networkevents.h>
#include <fircc/networkeventdispatchers.h>

namespace anp
{
namespace irc
{
	class NetworkFactory;
	
	// Abbreviations/prefixes:
	// pf - plugin function, implemented in plugins
	
	class MsgPrefix;


	// Typedefs
	
	/**
	 * This function is called when a plugin is first loaded.
	 * 
	 * @param networkFactory
	 * The NetworkFactory assigned from the frontend. Can be used to open
	 * new networks and/or subscribe to new networks as well as networks being
	 * removed.
	 * 
	 * @param appContext
	 * This parameter may be used by the application to pass application
	 * specific data to the plugin. However, for such needs, it is recommended
	 * that the application implements it's own plugin manager.
	 * 
	 * @return
	 * Success indicator. Should return 0 on success. It is allowed to throw
	 * exceptions too.
	 */
	typedef uint32 (*PF_pluginInit)(NetworkFactory &networkFactory, void *appContext);
	
	/**
	 * This function is called when a plugin is about to be unloaded.
	 * 
	 * @param reason
	 * The reason for the unload. Usage of this is currently undefined.
	 */
	typedef void (*PF_pluginDeinit)(uint32 reason);
} // namespace irc
} // namespace anp

#endif // _PLUGIN_FUNCTIONS_H_
