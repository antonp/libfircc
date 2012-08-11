#include <fircc/networkfactory.h>

#include <iostream>

extern "C" unsigned int pluginInit(anp::irc::NetworkFactory &networkFactory,
                                   void *appContext)
{
    std::cout << "p1 i" << std::endl;

    return 0;
}

extern "C" void pluginDeinit()
{
    std::cout << "p1 d" << std::endl;
}

