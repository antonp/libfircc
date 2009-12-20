#ifndef _USERINFO_H_
#define _USERINFO_H_

#include <string>

namespace anp
{
namespace firc
{
	class UserInfo
	{
	public:
		const std::string &name() const;
	private:
		std::string m_name;
	};
}
}

#endif // _USERINFO_H_
