/*

Copyright (c) 2010, Anton Petersson < anton (at) anp.nu >
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "networkfactory.h"
#include <networkeventdispatchers.h>
#include <network.h>
#include <list>
#include <stdexcept>

namespace anp
{
namespace irc
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
		
		void closeNetwork(const std::string &host, const std::string &port)
		{
			for ( std::list<INetwork *>::iterator i=m_networks.begin();
			      i != m_networks.end(); i++ )
			{
				if ( (*i)->host() == host && (*i)->port() == port )
				{
					events::RemovingNetwork removingNetworkEvent(*(*i));
					m_removingNetworkDispatcher.dispatch(removingNetworkEvent);
					delete (*i);
					m_networks.erase(i);
					return;
				}
			}
			throw std::runtime_error("closeNetwork() couldn't find network");			
		}
	
		EventDispatcher<
			ISubscriber<events::NewNetwork>,
			events::NewNetwork
		> m_newNetworkDispatcher;
		
		EventDispatcher<
			ISubscriber<events::RemovingNetwork>,
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
										  const std::string &port,
										  const std::string &nick,
										  const std::string &user,
										  const std::string &realName)
	{
		Network *network = new Network(host.c_str(),
									   port.c_str(),
									   nick,
									   user,
									   realName);
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
		m_impl->closeNetwork(host, port);
	}
	
	dispatchers::NewNetwork &
	NetworkFactory::eventDispatcherNewNetwork()
	{
		return m_impl->m_newNetworkDispatcher;
	}
		
	dispatchers::RemovingNetwork &
	NetworkFactory::eventDispatcherRemovingNetwork()
	{
		return m_impl->m_removingNetworkDispatcher;
	}
} // irc
} // anp
