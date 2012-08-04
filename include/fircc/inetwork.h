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

#ifndef _NETWORKMANAGER_FRONTEND_H_
#define _NETWORKMANAGER_FRONTEND_H_

#include <string>
#include <anpcode/eventdispatcher.h>
#include <fircc/networkeventdispatchers.h>
#include <fircc/networkevents.h>
#include <sys/select.h>

namespace anp
{
namespace irc
{
    class NetworkCacheUserInterface;

    /**
     * Network interface. Groups information about a IRC network together with
     * the connection to it.
     */
    class INetwork
    {
    public:
        INetwork() { }
        virtual ~INetwork() { }

        /**
        Sends the QUIT message to IRC and blocks until the receiver
        thread (if any) has exited.

        @param message
        The quit message to send to the IRC network.
        */
        virtual void deinit(const std::string &message) = 0;

        /**
        Sends a raw message to the IRC server.

        @param message
        The message to send. Should include the trailing CRLF as
        it is not added automatically by this method.
        */
        virtual void sendMessage(const std::string &message) = 0;

        /**
        Spawns a thread that calls runMessageReceiver.
        */
        virtual void runMessageReceiverInThread() = 0;

        /**
        Runs the message receiver in the current thread, causing it to
        block until the network is disconnected.
        */
        virtual void runMessageReceiver() = 0;

        /**
        Try receiving data and process it. Returns immediately
        if no data available.

        Hint: Use select() to determine if this network's socket
        can be read without blocking.

        @see addSocketToFdSet
        @see internalSocketInSet
        */
        virtual void tryReceive() = 0;

        /**
        Adds the read socket of the internal network connection to
        the supplied set of file descriptors.
        Use this method together with tryReceive to implement a
        single-threaded multi-connection client.

        @return The max descriptor, "nfds" parameter in the select() function.

        @see tryReceive
        @see internalSocketInSet
        */
        virtual int addSocketToFdSet(fd_set *readfds) = 0;

        /**
        Tests to see if the internal socket/file descriptor is part of the
        set.
        */
        virtual bool internalSocketInSet(fd_set *fds) = 0;

        /**
        Retrieves the name of the remote host that this network is connected to.

        @return
        The host name.
        */
        virtual const std::string &host() = 0;

        /**
        Retrieves the remote port that this network is connected to.

        @return
        The port.
        */
        virtual const std::string &port() = 0;

        // Network cache stuff

        /**
        Returns a reference to a NetworkCacheUserInterface which
        can be used to query the internal database for information
        about channels and users, but not to modify it.
        */
        virtual const NetworkCacheUserInterface &networkCache() const = 0;

        /**
        Returns a subscriber interface for the JOIN event dispatcher.
        */
        virtual dispatchers::Join &
        eventDispatcherJoin() = 0;

        /**
        Returns a subscriber interface for the PART event dispatcher.
        */
        virtual dispatchers::Part &
        eventDispatcherPart() = 0;

        /**
        Returns a subscriber interface for the PRIVMSG event dispatcher.
        */
        virtual dispatchers::PrivMsg &
        eventDispatcherPrivMsg() = 0;

        /**
        Returns a subscriber interface for the TOPIC event dispatcher.
        */
        virtual dispatchers::Topic &
        eventDispatcherTopic() = 0;

        /**
        Returns a subscriber interface for the numeric reply event
        dispatcher, which is a generic dispatcher for all numeric
        replies.
        */
        virtual dispatchers::NumericReply &
        eventDispatcherNumericReply() = 0;

        /**
        Returns a subscriber interface for the command event dispatcher,
        which is a generic dispatcher for all irc commands.
        */
        virtual dispatchers::Command &
        eventDispatcherCommand() = 0;

        /**
        Returns a subscriber interface for the PING event dispatcher.

        @remark
        No response is needed since the INetwork implementation should
        respond to this message automatically.
        */
        virtual dispatchers::Ping &
        eventDispatcherPing() = 0;

        /**
        Returns a subscriber interface for the ExceptionOccured event
        dispatcher.
        */
        virtual dispatchers::ExceptionOccured &
        eventDispatcherExceptionOccured() = 0;
    };
}
}

#endif // _NETWORKMANAGER_FRONTEND_H_
