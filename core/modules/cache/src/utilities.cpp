#include <userinfo.h>

namespace anp
{
namespace firc
{
	int userinfo_compare(const UserInfo *u1, const UserInfo *u2)
	{
		return u1->name().compare(u2->name());
	}
}
}
