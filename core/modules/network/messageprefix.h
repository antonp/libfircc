#ifndef _MESSAGE_PREFIX_H_
#define _MESSAGE_PREFIX_H_

#include <string>

namespace anp
{
namespace firc
{
class MsgPrefix
{
public:
	MsgPrefix(	const std::string &prefix,
				const std::string &nick,
				const std::string &user,
				const std::string &host):
	m_prefix(prefix),
	m_nick(nick),
	m_user(user),
	m_host(host)
	{
	}

	const std::string &prefix() const { return m_prefix; }
	const std::string &nick() const { return m_nick; }
	const std::string &user() const { return m_user; }
	const std::string &host() const { return m_host; }
protected:
	std::string m_nick;
	std::string m_user;
	std::string m_host;
	std::string m_prefix;
};
} // namespace firc
} // namespace anp

#endif // _MESSAGE_PREFIX_H_
