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
		
		class CooldownTime
		{
		public:
			CooldownTime():m_time(5000) { }
			void set(uint32 ms)
			{
				m_mutex.lock();
				m_time = ms;
				m_mutex.unlock();
			}
			void get(uint32 &value) const
			{
				m_mutex.lock();
				value = m_time;
				m_mutex.unlock();
			}
		private:
			mutable threading::Mutex m_mutex;
			uint32 m_time;
		} m_cooldownTime;
		
		//threading::ProtectedData<uint32, 5000> m_cooldownTime;
	};
}
}

#endif // _MESSAGESENDER_H_
