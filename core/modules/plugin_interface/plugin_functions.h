#ifndef _PLUGIN_FUNCTIONS_H_
#define _PLUGIN_FUNCTIONS_H_

namespace firc
{
	// Abbreviations/prefixes:
	// pf - plugin function, implemented in plugin but callable from core
	// cf - core function, implemented in core but callable from plugin
	
	// Typedefs
	// uint32 pluginInit(void *fircCore)
	typedef uint32 (*PF_pluginInit)(void *);
	
	// void pluginDeinit()
	typedef void (*PF_pluginDeinit)(uint32);
	
	// void irc_onJoin(void *network, const int8 *channel, const int8 *user)
	typedef void (*PF_irc_onJoin)(void *, const int8 *, const int8 *);
	
	// void irc_onPrivMsg(void *network, const int8 *sender, const int8 *receiver, const int8 *message)
	typedef void (*PF_irc_onPrivMsg)(void *, const int8 *, const int8 *, const int8 *);
}

#endif // _PLUGIN_FUNCTIONS_H_
