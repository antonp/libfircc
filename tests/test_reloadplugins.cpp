
#include <fircc/pluginmanager.h>
#include <fircc/networkfactory.h>

void test_reload()
{
    anp::irc::NetworkFactory factory;
    anp::irc::PluginManager manager;

    manager.loadPlugin("./libtestimpl_plugin1.so", factory, NULL);
    manager.unloadPlugin("./libtestimpl_plugin1.so", 0);
    manager.loadPlugin("./libtestimpl_plugin1.so", factory, NULL);
    manager.loadPlugin("./libtestimpl_plugin2.so", factory, NULL);
    manager.unloadPlugin("./libtestimpl_plugin1.so", 0);
    manager.unloadPlugin("./libtestimpl_plugin2.so", 0);
}


int main(int argc, char *argv[])
{
    test_reload();

    return 0;
}
