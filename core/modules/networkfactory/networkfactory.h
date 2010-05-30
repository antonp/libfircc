#ifndef _NETWORKFACTORY_H_
#define _NETWORKFACTORY_H_

#include <string>
#include <eventdispatcher.h>
#include <networkevents.h> /// @todo hide this: forward declare events and move ISubscriber out of 'events' namespace
#include <networkeventdispatchers.h>

namespace anp
{
namespace firc
{
	class INetwork;
	class NetworkFactoryImpl;
	
	class NetworkFactory
	{
	public:
		NetworkFactory();
		~NetworkFactory();

		INetwork *openNetwork(const std::string &host,
							  const std::string &port);

		void closeNetwork(INetwork *network);
		void closeNetwork(const std::string &host,
						  const std::string &port);
						  
		dispatchers::NewNetwork &eventDispatcherNewNetwork();
		dispatchers::RemovingNetwork &eventDispatcherRemovingNetwork();
	private:
		NetworkFactoryImpl *m_impl;
	};
} // firc
} // anp

#endif // _NETWORKFACTORY_H_
