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

#ifndef _NETWORKFACTORY_H_
#define _NETWORKFACTORY_H_

#include <string>
#include <eventdispatcher.h>
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
							  const std::string &port,
							  const std::string &nick,
							  const std::string &user,
							  const std::string &realName);

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
