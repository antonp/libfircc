#ifndef _MESSAGESENDER_H_
#define _MESSAGESENDER_H_

#include <basedefs.h>
#include <anp_threadsafequeue.h>

namespace anp
{
namespace firc
{
	class TCPConnection;
	
	class MessageSender
	{
	public:
		MessageSender(TCPConnection &m_connection);

		void run();
	private:
		TCPConnection &m_connection;
		ThreadSafeQueue m_queue;
	};
}
}

#endif // _MESSAGESENDER_H_
