#ifndef _FIRCAPI_H_
#define _FIRCAPI_H_

#include <basedefs.h>
#include <plugin_functions.h>

/**
 * @file fircapi.h
 * Plugins include this file to interact with the core functionality
 */

namespace anp
{
namespace firc
{
	
	typedef void *ChannelInfo;
	
extern "C"
{
	/* Core API */
	Result coreCreate(void **fircCore, uint8 pluginCount, 
						const int8 *pluginNames[]);
	
	void coreDestroy(void *fircCore);
	
	Result coreGetPluginManager(void *fircCore, void **pluginManager);
	
	/* IRC API: Basics */
	Result ircConnect(void *fircCore, const int8 *host,
						const int8 *port, void **network);
						
	Result ircDisconnect(void *fircCore, void *network,
							const int8 *message);
	
	void ircPrivMsg(void *network, const int8 *target,
					const int8 *message);
	void ircSendRaw(void *network, const int8 *rawCommand);
	
	Result ircAddOnPrivMsgCallback(void *fircCore,
									PF_irc_onPrivMsg func);
	
	/* IRC API: Network cache */
	Result cacheGetBotNickName(void *network, int8 *destName,
								uint32 maxNameLength);
	
	Result cacheGetTopic(void *network, const int8 *channel,
						int8 *destTopic, uint32 maxTopicSize);
						
	Result cacheGetChannelUserCount(void *network, const int8 *channel,
									uint32 *destUserCount);
	
	// Untested
	Result cacheIsUserInChannel(void *network, const int8 *channel,
								const int8 *userNick,
								bool32 userIsInChannel);

	/* ChannelInfo API */
	Result cacheCreateChannelInfoFromChannelName(void *network,
								const int8 *channel,
								const ChannelInfo *channelInfo);
				
	void cacheDestroyChannelInfo(ChannelInfo channelInfo);
	
	Result channelInfoGetTopic(ChannelInfo channelInfo, int8 *destTopic,
								uint32 maxTopicSize);
	
	Result channelInfoGetUserCount(ChannelInfo channelInfo,
									uint32 *userCount);
	
	Result channelInfoGetUserNick(ChannelInfo channelInfo, uint32 index,
									const int8 **destNickName);
									
	Result channelInfoGetUserString(ChannelInfo channelInfo,
										uint32 index,
										const int8 **destUserString);

	Result channelInfoGetUserHost(ChannelInfo channelInfo,
										uint32 index,
										const int8 **destHost);
	
// todo
// api to retrieve a user from an index (perhaps not just name but an
// object from which more info can be retrieved!)
	
// todo
//	Result channelInfoGetName(ChannelInfo channelInfo, int8 *destName,
//								uint32 maxNameSize);
	
	/* Plugin related API */
	Result pluginLoad(void *pluginManager, const int8 *fileName);
	
	Result pluginGetPluginCount(void *pluginManager, uint32 *count);
	Result pluginGetPluginInfo(void *pluginManager, uint32 index,
								int8 *name, uint32 nameLength);

	Result pluginUnload(void *pluginManager, uint32 index,
						uint32 reason);
} // extern "C"
} // namespace firc
} // namespace anp

#endif // _FIRCAPI_H_
