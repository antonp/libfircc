#include "../networkfactory.h"
#include <networkeventdispatchers.h>
#include <network.h>
#include <list>
#include <stdexcept>

namespace anp
{
namespace firc
{
	class NetworkFactoryImpl
	{
	public:
		NetworkFactoryImpl()
		{
		}
	
		~NetworkFactoryImpl()
		{
			std::list<INetwork *>::iterator i;
			for ( i=m_networks.begin(); i != m_networks.end(); i++ )
			{
				try
				{
					events::RemovingNetwork removingNetworkEvent(*(*i));
					m_removingNetworkDispatcher.dispatch(removingNetworkEvent);
					delete (*i);
				} catch ( std::exception &e )
				{
					// drop/log
				}
			}
		}
		
		void closeNetwork(INetwork *network)
		{
			for ( std::list<INetwork *>::iterator i=m_networks.begin();
			      i != m_networks.end(); i++ )
			{
				if ( (*i) == network )
				{
					events::RemovingNetwork removingNetworkEvent(*network);
					m_removingNetworkDispatcher.dispatch(removingNetworkEvent);
					delete network;
					m_networks.erase(i);
					return;
				}
			}
			throw std::runtime_error("closeNetwork() couldn't find network");
		}
	
		EventDispatcher<
			events::ISubscriber<events::NewNetwork>,
			events::NewNetwork
		> m_newNetworkDispatcher;
		
		EventDispatcher<
			events::ISubscriber<events::RemovingNetwork>,
			events::RemovingNetwork
		> m_removingNetworkDispatcher;
		
		std::list<INetwork *> m_networks;
	};

	NetworkFactory::NetworkFactory(): m_impl(new NetworkFactoryImpl)
	{
	}

	NetworkFactory::~NetworkFactory()
	{
		delete m_impl;
	}

	INetwork *NetworkFactory::openNetwork(const std::string &host,
										  const std::string &port)
	{
		Network *network = new Network(host.c_str(), port.c_str());
		m_impl->m_networks.push_back(network);
		
		events::NewNetwork newNetworkEvent(*network);
		m_impl->m_newNetworkDispatcher.dispatch(newNetworkEvent);
		return network;
	}
	
	void NetworkFactory::closeNetwork(INetwork *network)
	{
		m_impl->closeNetwork(network);
	}

	void NetworkFactory::closeNetwork(const std::string &host,
									  const std::string &port)
	{
		/// @todo implement (or remove?)
		throw std::runtime_error("NetworkFactory::closeNetwork(h, p) not implemented.");
	}
	
	network::NewNetworkEventDispatcher &
	NetworkFactory::eventDispatcherNewNetwork()
	{
		return m_impl->m_newNetworkDispatcher;
	}
		
	network::RemovingNetworkEventDispatcher &
	NetworkFactory::eventDispatcherRemovingNetwork()
	{
		return m_impl->m_removingNetworkDispatcher;
	}
} // firc
} // anp
