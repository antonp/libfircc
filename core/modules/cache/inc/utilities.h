#ifndef _UTILITIES_H_
#define _UTILITIES_H_

namespace anp
{
namespace firc
{
	int userinfo_compare(const UserInfo *u1, const UserInfo *u2);
	bool channelinfo_compare(const ChannelCache *c1,
							const ChannelCache *c2);
}
}

#endif // _UTILITIES_H_
