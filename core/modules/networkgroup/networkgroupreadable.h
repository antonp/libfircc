#ifndef _NETWORKGROUPREADABLE_H_
#define _NETWORKGROUPREADABLE_H_

#include <basedefs.h>

namespace anp
{
namespace firc
{
	class NetworkManager;
	
	class INetworkGroupReadable
	{
	public:
		virtual uint32 getSize() const = 0;
		virtual NetworkManager *getNetworkManagerAt(uint32 index) = 0;
		virtual const NetworkManager *getNetworkManagerAt(uint32 index)
															const = 0;
	};
}
}

#endif // _NETWORKGROUPREADABLE_H_
