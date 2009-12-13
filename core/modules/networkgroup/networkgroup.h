#ifndef _NETWORKGROUP_H_
#define _NETWORKGROUP_H_

#include <basedefs.h>
#include <networkmanager_frontend.h>
#include <vector>

namespace anp
{
namespace firc
{
	class NetworkManagerFrontend;
	
	class NetworkGroup
	{
	public:
		void addNetworkManager(NetworkManagerFrontend *networkManager);
		uint32 getSize() const;
		NetworkManagerFrontend *getNetworkManagerAt(uint32 index);
		const NetworkManagerFrontend *getNetworkManagerAt(
													uint32 index) const;
	private:
		std::vector<NetworkManagerFrontend *> m_networks;
	};
}
}

#endif // _NETWORKGROUP_H_
