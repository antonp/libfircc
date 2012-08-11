
#include <fircc/inetwork.h>
#include <fircc/networkevents.h>
#include <fircc/networkeventdispatchers.h>
#include <fircc/networkfactory.h>
#include <fircc/networkeventsubscribers.h>
#include "inc/network_testimpl.h"

#include <iostream>

class EventHandler: public anp::irc::eventsubscribers::NewNetwork,
                    public anp::irc::eventsubscribers::RemovingNetwork,
                    public anp::irc::eventsubscribers::PrivMsg
{
public:
    void receiveEvent(anp::irc::events::NewNetwork &e)
    {
        std::cout << "NewNetwork event received" << std::endl;

        e.network().eventDispatcherPrivMsg().subscribe(this);
    }

    void receiveEvent(anp::irc::events::RemovingNetwork &e)
    {
        // nothing
    }

    void receiveEvent(anp::irc::events::PrivMsg &e)
    {
        std::cout << "PrivMsg event received: " << e.message() << std::endl;
    }
};

void test_newNetwork()
{
    anp::EventDispatcher<
        anp::ISubscriber<anp::irc::events::NewNetwork>,
        anp::irc::events::NewNetwork
    > disp;
    anp::irc::dispatchers::NewNetwork &ndisp = disp;

    EventHandler eventHandler;

    ndisp.subscribe(&eventHandler);

    NetworkTestImpl network;
    anp::irc::events::NewNetwork newNetworkEvent(network);
    disp.dispatch(newNetworkEvent);

    anp::irc::MsgPrefix origin("testnick!testusr@users.priv.test.com",
                               "testnick",
                               "testusr",
                               "users.priv.test.com");
    anp::irc::events::PrivMsg privMsgEvent(network,
                                           origin,
                                           "testtarget",
                                           "testmessage");
    network.m_privmsg.dispatch(privMsgEvent);

}

int main(int argc, char **argv)
{
    test_newNetwork();

    return 0;
}
