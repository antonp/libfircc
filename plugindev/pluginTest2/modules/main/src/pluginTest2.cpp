#include <basedefs.h>
#include <fircapi.h>
#include <iostream>
#include <sstream>
#include <string.h>

using namespace firc;

enum { TOPIC_SIZE = 128 };

extern "C"
{

void irc_onPrivMsg(void *network, const int8 *sender,
					const int8 *receiver, const int8 *message)
{
	Result res = RES_OK;
	int8 topic[TOPIC_SIZE] = { 0 };
	
	if ( 0 == strcmp(message, "topic?") )
	{
		res = cacheGetTopic(network, receiver, topic, TOPIC_SIZE);
		if ( RES_OK == res )
		{
			ircPrivMsg(network, receiver, topic);
		} else
		{
			ircPrivMsg(network, receiver, "Failed to retrieve topic.");
		}
	} else if ( 0 == strcmp(message, "botnick?") )
	{
		res = cacheGetBotNickName(network, topic, TOPIC_SIZE);
		if ( RES_OK == res )
		{
			ircPrivMsg(network, receiver, topic);
		} else
		{
			ircPrivMsg(network, receiver, "Failed to retrieve bot nick.");
		}
	} else if ( 0 == strcmp(message, "usercount?") )
	{
		uint32 userCount = 0;
		res = cacheGetChannelUserCount(network, receiver, &userCount);
		if ( RES_OK == res )
		{
			std::stringstream ss;
			
			ss << "Users in channel: " << userCount << ".";
			ircPrivMsg(network, receiver, ss.str().c_str());
		} else
		{
			ircPrivMsg(network, receiver, "Failed to retrieve user count.");
		}
	} else if ( 0 == strcmp(message, "createChannelInfo!") )
	{
		ChannelInfo channelInfo;
		res = cacheCreateChannelInfoFromChannelName(network, receiver,
													&channelInfo);
		if ( RES_OK == res )
		{
			res = channelInfoGetTopic(channelInfo, topic, TOPIC_SIZE);
			if ( RES_OK == res )
			{
				ircPrivMsg(network, receiver, topic);
			}
		}
		
		cacheDestroyChannelInfo(channelInfo);
	} else if ( 0 == strcmp(message, "userCount?") )
	{
		ChannelInfo channelInfo;
		uint32 userCount = 0;
		res = cacheCreateChannelInfoFromChannelName(network, receiver,
													&channelInfo);
		if ( RES_OK == res )
		{
			res = channelInfoGetUserCount(channelInfo, &userCount);
			if ( RES_OK == res )
			{
				std::stringstream ss;
				ss << userCount;
				ircPrivMsg(network, receiver, ss.str().c_str());
			}
		}
		cacheDestroyChannelInfo(channelInfo);
	} else if ( 0 == strcmp(message, "userList?") )
	{
		ChannelInfo channelInfo;
		const int8 *tempStr = NULL;
		uint32 userCount = 0;
		
		res = cacheCreateChannelInfoFromChannelName(network, receiver,
													&channelInfo);
		if ( RES_OK == res )
		{
			res = channelInfoGetUserCount(channelInfo,&userCount);
			if ( RES_OK == res )
			{
				std::stringstream ss;
				
				for ( uint32 i=0; i<userCount; ++i )
				{
					res = channelInfoGetUserNick(channelInfo, i,
													&tempStr);
					if ( RES_OK == res )
					{
						ss << tempStr << '!';
					}
					res = channelInfoGetUserString(channelInfo, i,
													&tempStr);
					if ( RES_OK == res )
					{
						ss << tempStr << '@';
					}
					res = channelInfoGetUserHost(channelInfo, i,
													&tempStr);
					if ( RES_OK == res )
					{
						ss << tempStr << ',';
					}
				}
				ircPrivMsg(network, receiver, ss.str().c_str());
			}
		}
		cacheDestroyChannelInfo(channelInfo);
	}
}

uint32 pluginInit(void *fircCore)
{
	std::cout << "Hello world from pluginTest2.cpp" << std::endl;
}

void pluginDeinit(uint32 reason)
{
	std::cout << "Bye world from pluginTest2.cpp" << std::endl;
	std::cout << "Unloaded because: " << reason << std::endl;
}

}
