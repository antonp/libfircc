#include <userinfo.h>
#include <channelcache.h>

namespace anp
{
namespace firc
{
	int userinfo_compare(const UserInfo *u1, const UserInfo *u2)
	{
		return u1->name().compare(u2->name());
	}

	int channelinfo_compare(const ChannelCache *c1, const ChannelCache *c2)
	{
		return c1->name().compare(c2->name());
	}
}
}
