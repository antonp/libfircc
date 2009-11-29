#ifndef _MESSAGESENDER_H_
#define _MESSAGESENDER_H_

#include <basedefs.h>

namespace anp
{
namespace firc
{
	class TCPConnection;
	
	class MessageSender
	{
	public:
		MessageSender(TCPConnection &m_connection);

	private:
		TCPConnection &m_connection;
		
	};
}
}

#endif // _MESSAGESENDER_H_
