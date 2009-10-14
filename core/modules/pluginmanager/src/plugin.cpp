#include "../inc/plugin.h"

#include <dlfcn.h>
#include <iostream>

namespace anp
{
namespace firc
{
	/**
	 * @brief
	 * Simple constructor
	 */
	Plugin::Plugin():
		m_handle(NULL),
		m_name("n/a"),
		m_pf_irc_onJoin(NULL),
		m_pf_irc_onPrivMsg(NULL),
		m_executionCount(0),
		m_unloading(FALSE)
	{
		m_pf_pluginDeinit = NULL;
	}
	
	/**
	 * @brief
	 * Load plugin from file.
	 * 
	 * @param fircCore
	 * The core loading the plugin.
	 * 
	 * @param fileName
	 * NULL terminated string representing the fileName
	 * 
	 * @param[out] ioj
	 * Address of an irc_onJoin function pointer.
	 * 
	 * @return
	 * Returns RES_OK on success or a specific error code on failure.
	 */
	Result Plugin::loadFromFile(void *fircCore, const int8 *fileName,
								PF_irc_onJoin *ioj,
								PF_irc_onPrivMsg *iopm)
	{
		std::cout << "Plugin::loadFromFile: " << fileName << std::endl;
		m_handle = dlopen(fileName, RTLD_LAZY); // RTLD_NOW ?
		if ( NULL == m_handle )
		{
			/// @todo Check if m_handle == any of the
			///  previously loaded plugins
			std::cout << "dlopen failed:" << dlerror() << std::endl;
			return RES_FAILED;
		}
		
		PF_pluginInit pf_pluginInit	= (PF_pluginInit)dlsym(m_handle,
														"pluginInit");
		m_pf_pluginDeinit			= (PF_pluginDeinit)dlsym(m_handle,
														"pluginDeinit");
		
		if ( NULL == pf_pluginInit || NULL == m_pf_pluginDeinit )
		{
			std::cout << "functions missing:" << dlerror() << std::endl;
			dlclose(m_handle);
			return RES_FAILED;
		}
		
		// Find all implemented event handlers
		*ioj = m_pf_irc_onJoin =
			(PF_irc_onJoin)dlsym(m_handle, "irc_onJoin");
		*iopm = m_pf_irc_onPrivMsg =
			(PF_irc_onPrivMsg)dlsym(m_handle, "irc_onPrivMsg");
		
		// Call init function
		m_name = fileName;
		uint32 res = pf_pluginInit(fircCore);
		if ( res == 0 )
		{
			std::cout << "pluginInit returned 0" << std::endl;
			dlclose(m_handle);
			return RES_FAILED;
		}
		
		return RES_OK;
	}
	
	Result Plugin::unload(uint32 reason)
	{
		if ( m_executionCount == 0 )
		{
			m_pf_pluginDeinit(reason);
			dlclose(m_handle);
			return RES_OK;
		} else
		{
			return RES_FAILED;
		}
	}
	
	void Plugin::setUnloading(bool32 unloading)
	{
		m_unloading = unloading;
	}
	
	bool32 Plugin::isUnloading() const
	{
		return m_unloading;
	}
	
	Result Plugin::getName(const std::string **name)
	{
		Result res = RES_INVALID_PARAMETER;

		if ( NULL != name )
		{
			// Trust that the caller respects the constness
			*name = &m_name;
			res = RES_OK;
		}
		return res;
	}
	
	/////////////////////////
	// Get plugin handlers
	/////////////////////////

	Result Plugin::getEventHandler(PF_irc_onJoin *dest) const
	{
		Result res = RES_INVALID_PARAMETER;
		
		if ( NULL != dest )
		{
			if ( NULL != m_pf_irc_onJoin )
			{
				*dest = m_pf_irc_onJoin;
				res = RES_OK;
			} else
			{
				res = RES_NOTFOUND;
			}
		}
		
		return res;
	}
	
	Result Plugin::getEventHandler(PF_irc_onPrivMsg *dest) const
	{
		Result res = RES_INVALID_PARAMETER;
		
		if ( NULL != dest )
		{
			if ( NULL != m_pf_irc_onPrivMsg )
			{
				*dest = m_pf_irc_onPrivMsg;
				res = RES_OK;
			} else
			{
				res = RES_NOTFOUND;
			}
		}
		
		return res;
	}
	
	//////////////////////////////////
	// Execution, status, reloading
	//////////////////////////////////

	void Plugin::increaseExecutionCount()
	{
		++m_executionCount;
	}
	
	void Plugin::decreaseExecutionCount()
	{
		--m_executionCount;
		
		/// @todo Add assert and/or log if m_executionCount < 0
	}
} // namespace firc
} // namespace anp
