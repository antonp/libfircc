#ifndef _PLUGIN_H_
#define _PLUGIN_H_

#include <basedefs.h>

namespace firc
{
	// Abbreviations/prefixes:
	// pf - plugin function, implemented in plugin but callable from core
	// cf - core function, implemented in core but callable from plugin
	
	// Typedefs
	// uint32 pluginInit()
	typedef uint32 (*PF_pluginInit)();
	
	typedef void (*PF_pluginDeinit)();
	
	class Plugin
	{
	public:
		Plugin();
		
		Result loadFromFile(const int8 *fileName);
		void unload();
	private:
		void			*m_handle;
		
		PF_pluginDeinit m_pf_pluginDeinit;
	};
}

#endif // _PLUGIN_H_
