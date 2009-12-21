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
		UserInfo(const std::string &name, const std::string &user,
											const std::string &host);
		const std::string &name() const;
	private:
		std::string m_name;
		std::string m_user;
		std::string m_host;
	};
}
}

#endif // _USERINFO_H_
