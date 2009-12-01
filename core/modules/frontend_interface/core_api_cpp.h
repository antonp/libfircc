#ifndef _CORE_API_CPP_H_
#define _CORE_API_CPP_H_

namespace anp
{
namespace firc
{
	ICoreInterface *core_create(uint8 pluginCount, const int8 *pluginNames[]);
	void core_destroy(ICoreInterface *core);
}
}

#endif // _CORE_API_CPP_H_
