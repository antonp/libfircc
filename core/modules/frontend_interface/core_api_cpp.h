#ifndef _CORE_API_CPP_H_
#define _CORE_API_CPP_H_

#include <basedefs.h>

namespace anp
{
namespace firc
{
	class ICoreFrontend;
	
	ICoreFrontend *core_create(uint8 pluginCount, const int8 *pluginNames[]);
	void core_destroy(ICoreFrontend *core);
}
}

#endif // _CORE_API_CPP_H_
