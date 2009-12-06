#include <anp_dynamic_library.h>
#include <dlfcn.h>
#include <stdexcept>
#include <sstream>

namespace anp
{
	class DynamicLibraryImplementation
	{
	public:
		void *m_handle;
	
		DynamicLibraryImplementation(const int8 *fileName):
		m_handle(dlopen(fileName, RTLD_LAZY)) // RTLD_NOW ?
		{
			if ( NULL == m_handle )
			{
				std::stringstream errorMsg;
				errorMsg << "dlopen failed:" << dlerror() << std::endl;
				throw std::runtime_error(errorMsg.str());
			}
		}
		
		~DynamicLibraryImplementation()
		{
			dlclose(m_handle);
		}
		
		void *getSymbol(const int8 *symbolName)
		{
			void *symbol = dlsym(m_handle, symbolName);
			if ( NULL == symbol )
			{
				std::stringstream errorMsg;
				errorMsg << "dlsym returned NULL: "
					<< dlerror() << std::endl;
				throw std::runtime_error(errorMsg.str());
			}
		}
	};

	
	// DynamicLibrary function bodies
	DynamicLibrary::DynamicLibrary(const int8 *fileName):
	m_impl(new DynamicLibraryImplementation(fileName))
	{
		
	}
	DynamicLibrary::~DynamicLibrary()
	{
		delete m_impl;
	}
		
	void *DynamicLibrary::getSymbol(const int8 *symbolName)
	{
		return m_impl->getSymbol(symbolName);
	}
}
