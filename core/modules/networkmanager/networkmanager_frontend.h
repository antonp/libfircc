#ifndef _NETWORKMANAGER_FRONTEND_H_
#define _NETWORKMANAGER_FRONTEND_H_

#include <string>

namespace anp
{
namespace firc
{
	class INetworkManagerFrontend
	{
	public:
		INetworkManagerFrontend() { }
		virtual ~INetworkManagerFrontend() { }

		virtual void sendMessage(const std::string &message) = 0;
		
		// Network cache stuff (getCache() instead ?)
	};
}
}

#endif // _NETWORKMANAGER_FRONTEND_H_
