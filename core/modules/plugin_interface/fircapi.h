#ifndef _FIRCAPI_H_
#define _FIRCAPI_H_

#include <basedefs.h>

/**
 * @file fircapi.h
 * Plugins include this file to interact with the core functionality
 */

namespace firc
{
extern "C"
{
	void /*__attribute__ ((__visibility__("default")))*/ ircPrivMsg(void *network, const int8 *target,
					const int8 *message);
	void /*__attribute__ ((__visibility__("default")))*/ ircSendRaw(void *network, const int8 *rawCommand);
} // extern "C"
}

#endif // _FIRCAPI_H_
