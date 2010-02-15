#ifndef _NETWORKCACHE_USERINTERFACE_H_
#define _NETWORKCACHE_USERINTERFACE_H_

namespace anp
{
namespace firc
{
	class ChannelCache;
	
	class NetworkCacheUserInterface
	{
	public:
		virtual ~NetworkCacheUserInterface() { }

		/**
		Returns a ChannelCache which can be used to retrieve
		information about a particular channel.

		@param name
		The name of the channel.

		@return
		Pointer to ChannelCache.
		*/
		virtual const ChannelCache *getChannel(
									const std::string &name) const = 0;

		/**
		Copy information about a particular channel into an
		already allocated ChannelCache object.

		@param name
		The name of the channel.

		@param dest
		The channel information will be written to this ChannelCache
		object.
		*/
		virtual void getChannel(const std::string &name,
										ChannelCache &dest) const = 0;

		/**
		Copies the client nick name into the clientNickName
		parameter.

		@param clientNickName
		The client nickname will be written to this string.
		*/
		virtual void getClientNickName(std::string &clientNickName)
															const = 0;
	};
}
}

#endif // _NETWORKCACHE_USERINTERFACE_H_
