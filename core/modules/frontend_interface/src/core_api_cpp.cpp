#include <basedefs.h>
#include <core.h>

namespace anp
{
namespace firc
{
	ICoreFrontend *core_create(uint8 pluginCount,
							   const int8 *pluginNames[])
	{
		return new Core(pluginCount, pluginNames);
	}
	
	void core_destroy(ICoreFrontend *core)
	{
		delete core;
	}
}
}
