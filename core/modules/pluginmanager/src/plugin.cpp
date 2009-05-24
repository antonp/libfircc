#include "../inc/plugin.h"

#include <dlfcn.h>
#include <iostream>

namespace firc
{
	/**
	 * @brief
	 * Simple constructor
	 */
	Plugin::Plugin():
		m_handle(NULL)
	{
		m_pf_pluginDeinit = NULL;
	}
	
	/**
	 * @brief
	 * Load plugin from file.
	 * 
	 * @param fileName
	 * NULL terminated string representing the fileName
	 * 
	 * @return
	 * Returns RES_OK on success or a specific error code on failure.
	 */
	Result Plugin::loadFromFile(const int8 *fileName)
	{
		std::cout << "Plugin::loadFromFile: " << fileName << std::endl;
		m_handle = dlopen(fileName, RTLD_LAZY); // RTLD_NOW ?
		if ( NULL == m_handle )
		{
			std::cout << "dlopen failed:" << dlerror() << std::endl;
			return RES_FAILED;
		}
		
		PF_pluginInit pf_pluginInit	= (PF_pluginInit)dlsym(m_handle,
														"pluginInit");
//		m_pf_pluginDeinit			= (PF_pluginDeinit)dlsym(m_handle,
//														"pluginDeinit");
		
		if ( NULL == pf_pluginInit /*|| NULL == m_pf_pluginDeinit*/ )
		{
			std::cout << "functions missing:" << dlerror() << std::endl;
			dlclose(m_handle);
			return RES_FAILED;
		}
		
		// Call init function
		uint32 res = pf_pluginInit();
		if ( res != 0 )
		{
			std::cout << "pluginInit returned 0" << std::endl;
			dlclose(m_handle);
			return RES_FAILED;
		}
		
		return RES_OK;
	}
	
	void Plugin::unload()
	{
		dlclose(m_handle);
	}
}
