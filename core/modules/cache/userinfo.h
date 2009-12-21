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
		const std::string &user() const;
		const std::string &host() const;
		const std::string &userString() const;
	private:
		void updateUserString() const;
	
		std::string m_name;
		std::string m_user;
		std::string m_host;
		mutable std::string m_userString;
		mutable bool32 m_userStringIsDirty;
	};
}
}

#endif // _USERINFO_H_
