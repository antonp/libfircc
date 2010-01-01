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

		virtual const ChannelCache *getChannel(
									const std::string &name) const = 0;
		virtual void getClientNickName(std::string &clientNickName)
															const = 0;
	};
}
}

#endif // _NETWORKCACHE_USERINTERFACE_H_
