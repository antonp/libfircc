#ifndef _NETWORKCACHE_H_
#define _NETWORKCACHE_H_

namespace anp
{
namespace firc
{
	class NetworkCacheImpl;
	
	class NetworkCache
	{
	public:
		NetworkCache();
		~NetworkCache();
	private:
		NetworkCacheImpl *m_impl;
	};
}	
}

#endif // _NETWORKCACHE_H_
