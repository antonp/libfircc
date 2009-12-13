#include <networkgroup.h>
#include <vector>

namespace anp
{
namespace firc
{
		void NetworkGroup::addNetworkManager(
								NetworkManagerFrontend *networkManager)
		{
			m_networks.push_back(networkManager);
		}
		
		uint32 NetworkGroup::getSize() const
		{
			return m_networks.size();
		}
		NetworkManagerFrontend *
		NetworkGroup::getNetworkManagerAt(uint32 index)
		{
			return m_networks.at(index);
		}
		
		const NetworkManagerFrontend *
		NetworkGroup::getNetworkManagerAt(uint32 index) const
		{
			return m_networks.at(index);
		}
}
}
