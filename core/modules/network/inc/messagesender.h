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
		MessageSender(TCPConnection &connection);
		virtual ~MessageSender();

		void addMessage(const std::string &message);
		
		void setCooldownTime(uint32 ms);
		
		/**
		 * Forces the monitor to stop, without letting it
		 * send the messages still in queue, if any.
		 */
		void stop();
	private:
		// Should be hidden somehow.. private (for now) or maybe interfaced
		void monitor();
	
		TCPConnection &m_connection;
		threading::ThreadSafeQueue<std::string> m_queue;
		
		threading::ProtectedData<uint32, 5000> m_cooldownTime;
		threading::ProtectedData<bool32, false> m_isDying;
		
		threading::Event m_newMessage;
		threading::Thread m_thread;
	};
}
}

#endif // _MESSAGESENDER_H_
