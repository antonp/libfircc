#ifndef _DYNAMIC_LIBRARY_H_
#define _DYNAMIC_LIBRARY_H_

#include <basedefs.h>

namespace anp
{
	class DynamicLibraryImplementation;
	
	class DynamicLibrary
	{
	public:
		DynamicLibrary(const int8 *fileName);
		virtual ~DynamicLibrary();
		
		void *getSymbol(const int8 *symbolName);
	private:
		DynamicLibraryImplementation *m_impl;
	};
}

#endif // _DYNAMIC_LIBRARY_H_
