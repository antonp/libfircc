#ifndef _NETWORKFACTORY_H_
#define _NETWORKFACTORY_H_

#include <string>
#include <eventdispatcher.h>
#include <networkevents.h> // hide this: forward declare events and move ISubscriber out of 'events' namespace
#include <networkeventdispatchers.h>

namespace anp
{
namespace firc
{
	class Network;
	class NetworkFactoryImpl;
	
	class NetworkFactory
	{
	public:
		NetworkFactory();
		~NetworkFactory();

		Network *openNetwork(const std::string &host,
							 const std::string &port);

		void closeNetwork(Network *network);
		void closeNetwork(const std::string &host,
						  const std::string &port);
						  
		network::NewNetworkEventDispatcher &eventDispatcherNewNetwork();
		network::RemovingNetworkEventDispatcher &eventDispatcherRemovingNetwork();
	private:
		NetworkFactoryImpl *m_impl;
	};
} // firc
} // anp

#endif // _NETWORKFACTORY_H_
