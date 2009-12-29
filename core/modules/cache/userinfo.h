#ifndef _USERINFO_H_
#define _USERINFO_H_

#include <string>

namespace anp
{
namespace firc
{
	struct UserInfoImpl;

	class UserInfo
	{
	public:
		UserInfo(const std::string &name, const std::string &user,
											const std::string &host);
		~UserInfo();
		const std::string &name() const;
		const std::string &user() const;
		const std::string &host() const;
		const std::string &userString() const;
		
		void setName(const std::string &name);
		void setUser(const std::string &user);
		void setHost(const std::string &host);
	private:
		UserInfoImpl *m_impl;
	};
}
}

#endif // _USERINFO_H_
