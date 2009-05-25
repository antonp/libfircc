#ifndef _PLUGIN_H_
#define _PLUGIN_H_

#include <basedefs.h>
#include "plugin_functions.h"

namespace firc
{
	class Plugin
	{
	public:
		Plugin();
		
		Result loadFromFile(const int8 *fileName, PF_irc_onJoin *ioj);
		void unload();
	private:
		void				*m_handle;
		
		PF_pluginDeinit		m_pf_pluginDeinit;
		
		// Pure irc functions
		PF_irc_onJoin		m_pf_irc_onJoin;
		PF_irc_onPrivMsg	m_pf_irc_onPrivMsg;
	};
}

#endif // _PLUGIN_H_
