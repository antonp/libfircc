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

#include "inc/plugin.h"

#include <sstream>
#include <stdexcept>
#include <cassert>
#include <anpcode/anp_dynamic_library.h>

namespace anp
{
namespace irc
{	
	/**
	 * @brief
	 * Creates a plugin by loading it from file.
	 * 
	 * @param fileName
	 * NULL terminated string representing the fileName
	 */
	Plugin::Plugin(
		const char *fileName,
		NetworkFactory &networkFactory,
		void *appContext
	):
		m_lib(fileName),
		m_name("n/a"),
		m_pf_pluginDeinit(NULL),
		m_unloadReason(0)
	{		
		PF_pluginInit pf_pluginInit	= (PF_pluginInit)m_lib.getSymbol(
														"pluginInit");
		m_pf_pluginDeinit			= (PF_pluginDeinit)m_lib.getSymbol(
														"pluginDeinit");

		// DynamicLibrary::getSymbol should throw an exception if
		// any of the functions cannot be found, so this should never
		// happen.
		assert(NULL != pf_pluginInit && NULL != m_pf_pluginDeinit);
		
		// Call init function
		m_name = fileName;
		unsigned int res = pf_pluginInit(networkFactory, appContext);
		if ( res == 0 )
		{
			throw std::runtime_error("pluginInit returned 0");
		}
	}
	
	Plugin::~Plugin()
	{
		m_pf_pluginDeinit(m_unloadReason);
	}
	
	void Plugin::setUnloadReason(unsigned int reason)
	{
		m_unloadReason = reason;
	}
	
	void Plugin::setUnloading(bool unloading)
	{
		m_unloading = unloading;
	}
	
	bool Plugin::isUnloading() const
	{
		return m_unloading;
	}
	
	const std::string &Plugin::getName() const
	{
		return m_name;
	}
} // namespace irc
} // namespace anp
