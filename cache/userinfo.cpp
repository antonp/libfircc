/*

Copyright (c) 2010, Anton Petersson < anton (at) anp.nu >
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <basedefs.h>
#include "userinfo.h"
#include <string>
#include <sstream>

namespace anp
{
namespace irc
{
	struct UserInfoImpl
	{
		UserInfoImpl(const std::string &name, const std::string &user,
												const std::string &host):
		m_name(name),
		m_user(user),
		m_host(host),
		m_userStringIsDirty(true)
		{
		}
		
		void updateUserString() const
		{
			if ( m_userStringIsDirty )
			{
				std::stringstream ss;
				ss << m_name << '!' << m_user << '@' << m_host;
				m_userString = ss.str();
				m_userStringIsDirty = false;
			}
		}
	
		void invalidateUserString() const
		{
			m_userStringIsDirty = true;
		}

		std::string m_name;
		std::string m_user;
		std::string m_host;
		mutable std::string m_userString;
		mutable bool32 m_userStringIsDirty;
	};

	UserInfo::UserInfo(const std::string &name, const std::string &user,
											const std::string &host):
	m_impl(new UserInfoImpl(name, user, host))
	{
		m_impl->updateUserString();
	}

	UserInfo::~UserInfo()
	{
		delete m_impl;
	}
	
	const std::string &UserInfo::name() const
	{
		return m_impl->m_name;
	}
	
	const std::string &UserInfo::user() const
	{
		return m_impl->m_user;
	}
	
	const std::string &UserInfo::host() const
	{
		return m_impl->m_host;
	}
	
	const std::string &UserInfo::userString() const
	{
		m_impl->updateUserString();
		return m_impl->m_userString;
	}
	
	void UserInfo::setName(const std::string &name)
	{
		m_impl->m_name = name;
		m_impl->invalidateUserString();
	}
	
	void UserInfo::setUser(const std::string &user)
	{
		m_impl->m_user = user;
		m_impl->invalidateUserString();
	}
	
	void UserInfo::setHost(const std::string &host)
	{
		m_impl->m_host = host;
		m_impl->invalidateUserString();
	}
}
}
