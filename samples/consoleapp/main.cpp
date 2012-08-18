/*

Copyright (c) 2011, Anton Petersson < anton (at) anp.nu >
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

#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/socket.h>
#include <string.h>

#include <fircc/pluginmanager.h>
#include <fircc/inetwork.h>
#include <fircc/networkcache_userinterface.h>
#include <fircc/messageprefix.h>
#include <fircc/channelcache.h>
#include <fircc/networkevents.h>
#include <fircc/networkeventsubscribers.h>
#include <fircc/networkfactory.h>
#include <anpcode/eventdispatcher.h>
#include <anpcode/log_singleton.h>
#include <anpcode/stdvector.h>

// Some constants for easy modification.
static const char *const HOST_ADDR = "irc.freenode.net";
static const char *const HOST_PORT = "6667";
static const char *const NICK = "fircc_sample";
static const char *const USER = "fircc_user";
static const char *const REALNAME = "fircc_real";
static const char *const QUITMSG = "Time to go! See you!";
static const char *const LOGTAG = "fircc_sample";

// This is optional. It intercepts log messages
// and sends them to stdout.
class LogConsoleWriter: public anp::ILogInterface
{
public:
    LogConsoleWriter()
    {
    }
    void present(const std::string &tag,
                 const std::string &message,
                 const std::string &file,
                 const std::string &line)
    {
        std::cout << tag << " [" << file << ':' << line << "] " <<  message << std::endl;
    }
};

// This class will receive all the events from the irc network.
// You only need to implement receiveEvent for the events that
// you want to handle.
class EventHandler: public anp::irc::eventsubscribers::Join,
                    public anp::irc::eventsubscribers::Part,
                    public anp::irc::eventsubscribers::PrivMsg,
                    public anp::irc::eventsubscribers::Topic,
                    public anp::irc::eventsubscribers::NumericReply
{
public:
    void receiveEvent(anp::irc::events::Join &event)
    {
        // todo
    }
    void receiveEvent(anp::irc::events::Part &event)
    {
        // todo
    }
    void receiveEvent(anp::irc::events::PrivMsg &event)
    {
        if ( event.target()[0] == '#' )
        {
            const anp::irc::NetworkCacheUserInterface &cache =
                event.network().networkCache();
            if ( event.message() == "topic?" )
            {
                anp::irc::ChannelCache channel;
                cache.getChannel(event.target(), channel);

                std::stringstream ss;
                ss << "PRIVMSG " << channel.name() << " :The topic for "
                    << channel.name() << " is " << channel.topic() << ".\r\n";
                event.network().sendMessage(ss.str());
            } else if ( event.message() == "userlist?" )
            {
                anp::StdVector<anp::irc::NetworkCacheUserInterface::UserInChannel>
                    users;
                cache.getUsersInChannel(event.target(), users);

                std::stringstream ss;
                ss << "PRIVMSG " << event.target() << " :Users: ";
                
                for ( std::vector<anp::irc::NetworkCacheUserInterface::UserInChannel>::iterator i = users.m_vector.begin(); i != users.m_vector.end(); i++ )
                {
                    ss << '[' << (*i).modes << ']' << (*i).nick << ", ";
                }
                ss << "\r\n";
                event.network().sendMessage(ss.str());
            }
        }
    }
    void receiveEvent(anp::irc::events::Topic &event)
    {
        // todo
    }
    void receiveEvent(anp::irc::events::NumericReply &event)
    {
        // I'm using this event as a trigger for entering
        // a channel.
        if ( event.command() == "376" ) // RPL_ENDOFMOTD
        {
            anp::irc::INetwork &network = event.network();

            network.sendMessage("JOIN #libfircc\r\n");
            network.sendMessage(
                        "PRIVMSG #libfircc :Hello world!\r\n");
        }
    }
};

// The entrypoint for this sample app.
// You can pass it an address and a port
// to override the hardcoded values.
int main(int argc, char *argv[])
{
    std::string hostAddr = HOST_ADDR,
                hostPort = HOST_PORT;

    if ( argc >= 3 )
    {
        hostAddr = argv[1];
        hostPort = argv[2];
    }

    // Initialize logging
    LogConsoleWriter logConsoleWriter;
    anp::LogSingletonHelper log;
    log.addLogInterface(&logConsoleWriter);
    ANPLOGI(LOGTAG, "Log initialized.");

    // Connect to the irc network
    anp::irc::NetworkFactory networkFactory;
    anp::irc::INetwork *network = networkFactory.openNetwork(hostAddr,
                                                             hostPort,
                                                             NICK,
                                                             USER,
                                                             REALNAME);

    // Subscribe to events (optional)
    EventHandler eventHandler;
    network->eventDispatcherJoin().subscribe(&eventHandler);
    network->eventDispatcherPart().subscribe(&eventHandler);
    network->eventDispatcherPrivMsg().subscribe(&eventHandler);
    network->eventDispatcherTopic().subscribe(&eventHandler);
    network->eventDispatcherNumericReply().subscribe(&eventHandler);

    // Declare some variables to support i/o multiplexing (optional)
    // This allows us to wait for input from multiple sources without
    // using multiple threads.
    static const int TIMEOUT = 250000;
    timeval timeout;
    fd_set fds;

    int maxfds_network;
    int nfds;

    // When the quit command has been received, state will equal 1.
    while ( true )
    {
        FD_ZERO(&fds);
        FD_SET(fileno(stdin), &fds);
        maxfds_network = network->addSocketToFdSet(&fds);
        nfds = (maxfds_network > fileno(stdin) ? maxfds_network : fileno(stdin)) + 1;
        timeout.tv_sec = 0;
        timeout.tv_usec = TIMEOUT;

        // Wait for input through the network or stdin.
        if ( select(nfds, &fds, NULL, NULL, &timeout) > 0 )
        {
            if ( FD_ISSET(fileno(stdin), &fds) )
            {
                // Input available from stdin.
                std::string command;
                std::getline(std::cin, command);

                if ( command == "die" )
                {
                    // Shutdown command received, shutting down.
                    break;
                } else
                {
                    // Something else received, try sending it (raw)
                    // over the network.
                    network->sendMessage(command+"\r\n");
                }
            }
            if ( network->internalSocketInSet(&fds) )
            {
                // Input available from network - process it.
                // This might generate events, so our event
                // receiver might be called.
                network->tryReceive();
            }
        }
    }

    // Quit
    network->deinit(QUITMSG);
    networkFactory.closeNetwork(network);
    ANPLOGI(LOGTAG, "Successfully disconnected.");

    return 0;
}
