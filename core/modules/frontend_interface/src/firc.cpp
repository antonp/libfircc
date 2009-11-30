#include "../firc.h"
#include <networkmanager.h>
#include <anp_exceptions.h>

namespace anp
{
namespace firc
{
	Core::Core(uint8 pluginCount, const int8 *pluginNames[])
	{
		Result res = RES_FAILED;
		
		m_pluginManager.setFircCore((void *)this);
		res = m_pluginManager.loadPlugins(pluginCount,
									pluginNames);
		if ( RES_OK != res )
		{
			throw anp::ConstructionException("Failed to load plugins");
		}
	}
	
	Core::~Core()
	{
		for ( uint32 i=0; i<m_networkManagers.size(); ++i )
		{
			delete m_networkManagers[i];
		}
		m_networkManagers.clear();
	}
	
	bool32 Core::update()
	{
		return true;
	}
	
	NetworkManager *Core::createNetworkManager(const int8 *host,
												const int8 *port)
	{
		NetworkManager *nm(new NetworkManager(host, port,
							&m_pluginManager));
		m_networkManagers.push_back(nm);
		return nm;
	}
	
	Result Core::destroyNetworkManager(NetworkManager *networkManager,
										const int8 *message)
	{
		Result res = RES_INVALID_PARAMETER;

		if ( NULL != networkManager )
		{
			for ( uint32 i=0; i<m_networkManagers.size(); ++i )
			{
				if ( m_networkManagers[i] == networkManager )
				{
					m_networkManagers[i]->deinit(message);
					
					delete networkManager;
					m_networkManagers.erase(
										m_networkManagers.begin()+i);
					res = RES_OK;
				}
			}
		}
		return res;
	}
	
	Result Core::getPluginManager(PluginManager **pluginManager)
	{
		Result res = RES_INVALID_PARAMETER;
		
		if ( NULL != pluginManager )
		{
			*pluginManager = &m_pluginManager;
			res = RES_OK;
		}
		
		return res;
	}
	
	Result Core::addCallbackOnPrivMsg(PF_irc_onPrivMsg func)
	{
		Result res = RES_INVALID_PARAMETER;
		
		if ( NULL != func )
		{
			res = m_pluginManager.addCallbackOnPrivMsg(func);
		}
		return res;
	}
	
	/////////////////////
	
	extern "C"
	{
		/**
		 * @brief
		 * Creates a firc core object, wich is used to manage plugins
		 * and connections to different irc networks.
		 * 
		 * @param fircCore
		 * Address of a pointer that will receive the address of the
		 * new core object.
		 * 
		 * @param pluginCount
		 * Specifies the number of plugins in the pluginNames array.
		 * 
		 * @param pluginNames
		 * Specifies a list of plugins (including paths) to load.
		 * 
		 * @return
		 * Returns RES_OK on success or a specific error code on
		 * failure.
		 */
		Result coreCreate(void **fircCore, uint8 pluginCount, 
							const int8 *pluginNames[])
		{
			Result res = RES_INVALID_PARAMETER;
			
			if ( NULL != fircCore )
			{
				try
				{
					std::auto_ptr<Core> core(
									new Core(pluginCount, pluginNames));
					*fircCore = (void *)core.release();
					res = RES_OK;
				} catch ( std::exception &e )
				{
					res = RES_CONSTRUCTION_FAILED;
				}
			}
			
			return res;
		}
		
		/**
		 * @brief
		 * Destroys a firc core object. Disconnects all irc networks
		 * and unloads all plugins.
		 * 
		 * @param fircCore
		 * Pointer to the core object to destroy.
		 */
		void coreDestroy(void *fircCore)
		{
			/// @todo implement
		}
		
		/**
		 * @brief
		 * Retrieves a pointer to the internal plugin manager.
		 * 
		 * @param fircCore
		 * Pointer to a (the) firc core object.
		 * 
		 * @param pluginManager
		 * Address of a pointer that will be set to point to
		 * the plugin manager.
		 * 
		 * @return
		 * RES_OK on success or a specific error code on failure.
		 */
		Result coreGetPluginManager(void *fircCore,
									void **pluginManager)
		{
			Result res = RES_INVALID_PARAMETER;
			
			if ( NULL != fircCore && NULL != pluginManager )
			{
				res = ((Core *)fircCore)->getPluginManager(
									(PluginManager **)pluginManager);
			}
			
			return res;
		}
		
		/**
		 * @brief
		 * Connects to the specified network (in addition to already
		 * active networks).
		 * 
		 * @param fircCore
		 * Core object that will manage the connection.
		 * 
		 * @param host
		 * Hostname of the server/network to connect to.
		 * 
		 * @param port
		 * Target port to connect to, usually 6667. Specified as a
		 * string.
		 * 
		 * @param network
		 * Address of a pointer that will receive the address of the new
		 * network upon successful connection.
		 * 
		 * @return
		 * RES_OK on success or a specific error code on failure.
		 */
		Result ircConnect(void *fircCore, const int8 *host,
							const int8 *port, void **network)
		{
			Result res = RES_INVALID_PARAMETER;
			if ( NULL != fircCore && NULL != network )
			{
				Core *core = (Core *)fircCore;

				*network = core->createNetworkManager(host,	port);
			}
			return res;
		}
		
		/**
		 * @brief
		 * Disconnect from the specified network.
		 * 
		 * @param fircCore
		 * The core object handling this network.
		 * 
		 * @param message
		 * The quit message to send before leaving.
		 * 
		 * @return
		 * Returns RES_OK on success or a specific error code on
		 * failure.
		 */
		Result ircDisconnect(void *fircCore, void *network,
								const int8 *message)
		{
			Result res = RES_INVALID_PARAMETER;
			if ( NULL != fircCore && NULL != network )
			{
				Core *core = (Core *)fircCore;
				
				res = core->destroyNetworkManager(
						(NetworkManager *)network, message);
			}
			return res;
		}
		
		/**
		 * @brief
		 * Register a function that will be called everytime a PRIVMSG
		 * is received from one of the IRC networks.
		 * 
		 * @param func
		 * The function to call when a PRIVMSG is received.
		 * 
		 * @return
		 * Returns RES_OK on success or a specific error code on
		 * failure.
		 * 
		 * @remark
		 * The plugin _must_ unregister it's callback functions
		 * before it is unloaded, otherwise the core will try to
		 * call them and that will probably lead to a crash.
		 * 
		 * @sa ircRemoveOnPrivMsgCallback
		 */
		Result ircAddOnPrivMsgCallback(void *fircCore,
									PF_irc_onPrivMsg func)
		{
			Result res = RES_INVALID_PARAMETER;
			if ( NULL != fircCore && NULL != func )
			{
				res = ((Core *)fircCore)->addCallbackOnPrivMsg(func);
			}
			return res;
		}
	}
} // namespace firc
} // namespace anp
