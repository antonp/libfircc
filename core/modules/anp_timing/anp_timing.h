#ifndef _ANP_TIMING_H_
#define _ANP_TIMING_H_

#include <basedefs.h>

namespace anp
{
namespace timing
{
	uint32 getMilliseconds();
	
	void sleepMilliseconds(uint32 ms);
}
}

#endif // _ANP_TIMING_H_
