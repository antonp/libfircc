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
#include <anpcode/eventdispatcher.h>
#include <fircc/networkeventdispatchers.h>

namespace anp
{
namespace irc
{
    class INetwork;
    class NetworkFactoryImpl;

    /**
     * Creates and manages networks.
     *
     * Using NetworkFactory is optional, but has the benefit of automatically
     * dispatching an event about the new network, and also when a network is
     * removed.
     */
    class NetworkFactory
    {
    public:
        NetworkFactory();
        ~NetworkFactory();

        typedef void (*Visit)(INetwork &network, void *userData);

        /**
         * Opens a new network.
         *
         * @param host
         * Host to connect to.
         *
         * @param port
         * Remote port to connect to.
         *
         * @param nick
         * Nick name to use initially on this network.
         *
         * @param user
         * User name to use on this network.
         *
         * @param realName
         * Real name to use on this network.
         */
        INetwork *openNetwork(const std::string &host,
                              const std::string &port,
                              const std::string &nick,
                              const std::string &user,
                              const std::string &realName);

        /**
         * Closes a network.
         *
         * @param network
         * Network to close.
         *
         * @remark
         * The network will only be closed through the same
         * NetworkFactory that opened it.
         */
        void closeNetwork(INetwork *network);

        /**
         * Closes a network.
         *
         * @param host
         * Host of network to close.
         *
         * @param port
         * Remote port of network to close.
         *
         * @remark
         * The network will only be closed through the same
         * NetworkFactory that opened it.
         */
        void closeNetwork(const std::string &host,
                          const std::string &port);

        /**
         * Iterates over all INetwork instances associated
         * with this NetworkFactory instance.
         *
         * @param visit
         * This function will be called once for every network.

         * @param userData
         * Data to pass on to the supplied function
         */
        void foreachNetwork(Visit visit, void *userData);

        /**
         * Retrieves a subscriber interface for the NewNetwork event.
         *
         * @return
         * Returns a subscriber interface for the NewNetwork event.
         */
        dispatchers::NewNetwork &eventDispatcherNewNetwork();

        /**
         * Retrieves a subscriber interface for the RemovingNetwork event.
         *
         * @return
         * Returns a subscriber interface for the RemovingNetwork event.
         */
        dispatchers::RemovingNetwork &eventDispatcherRemovingNetwork();
    private:
        NetworkFactoryImpl *m_impl;
    };
} // irc
} // anp

#endif // _NETWORKFACTORY_H_
