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
	m_host(host),
	m_userStringIsDirty(true)
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
		updateUserString();
		return m_userString;
	}
	
	void UserInfo::setName(const std::string &name)
	{
		m_name = name;
		invalidateUserString();
	}
	
	void UserInfo::setUser(const std::string &user)
	{
		m_user = user;
		invalidateUserString();
	}
	
	void UserInfo::setHost(const std::string &host)
	{
		m_host = host;
		invalidateUserString();
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
	
	void UserInfo::invalidateUserString() const
	{
		m_userStringIsDirty = true;
	}
}
}
