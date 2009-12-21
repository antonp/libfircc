#include <basedefs.h>
#include "../userinfo.h"
#include <string>
#include <sstream>

namespace anp
{
namespace firc
{
	UserInfo::UserInfo(const std::string &name, const std::string &user,
											const std::string &host):
	m_name(name),
	m_user(user),
	m_host(host)
	{
		updateUserString();
	}
	
	const std::string &UserInfo::name() const
	{
		return m_name;
	}
	
	const std::string &UserInfo::user() const
	{
		return m_user;
	}
	
	const std::string &UserInfo::host() const
	{
		return m_host;
	}
	
	const std::string &UserInfo::userString() const
	{
		// assert(m_userStringIsDirty == false)
		return m_userString;
	}
	
	void UserInfo::updateUserString() const
	{
		if ( m_userStringIsDirty )
		{
			std::stringstream ss;
			ss << m_name << '!' << m_user << '@' << m_host;
			m_userString = ss.str();
			m_userStringIsDirty = false;
		}
	}
}
}
