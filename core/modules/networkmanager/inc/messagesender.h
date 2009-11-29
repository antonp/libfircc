#ifndef _MESSAGESENDER_H_
#define _MESSAGESENDER_H_

#include <basedefs.h>
#include <anp_threading.h>
#include <anp_threadsafequeue.h>
#include <string>

namespace anp
{
namespace firc
{
	class TCPConnection;
	
	class MessageSender
	{
	friend void *threadRunMessageSender(void *arg);
	public:
		MessageSender(TCPConnection &m_connection);

		void addMessage(const std::string &message);
		
		void setCooldownTime(uint32 ms);
		
		
	private:
		// Should be hidden somehow.. private (for now) or maybe interfaced
		void monitor();
	
		TCPConnection &m_connection;
		threading::ThreadSafeQueue<std::string> m_queue;
		
		threading::ProtectedData<uint32, 5000> m_cooldownTime;
	};
}
}

#endif // _MESSAGESENDER_H_
