#include "../inc/messagesender.h"

#include <anp_timing.h>
#include <anp_threadsafequeue.h>
#include <log_singleton.h>
#include <tcpconnection.h>
#include <sstream>

#include <stdexcept>

namespace anp
{
namespace firc
{
	void *threadRunMessageSender(void *arg)
	{
		MessageSender *ms = (MessageSender *)arg;
		if ( NULL == ms )
		{
			//throw std::invalid_argument("'Invalid MessageSender'");
			try
			{
				LogSingleton::getInstance().addMessage(std::string("MessageSender: Invalid MessageSender."));
				LogSingleton::releaseInstance();
			} catch ( ... )
			{
				// we're doomed
			}
			
		} else
		{
			try
			{
				ms->monitor();
			} catch ( std::exception &e )
			{
				ms->log(std::string("MessageSender: Exception occured: ")+e.what());
			}
		}
		pthread_exit(0);
	}
	
	MessageSender::MessageSender(TCPConnection &connection):
	m_connection(connection)
	{
		m_isDying.set(false);
		m_thread.create(NULL, threadRunMessageSender, (void *)this);
	}
	
	MessageSender::~MessageSender()
	{
		stop();
		m_thread.join(NULL);
	}
	
	void MessageSender::addMessage(const std::string &message)
	{
		m_queue.push(message);
		m_newMessage.signal();
	}

	void MessageSender::setCooldownTime(uint32 ms)
	{
		m_cooldownTime.set(ms);
	}
	
	void MessageSender::stop()
	{
		m_isDying.set(true);
		m_newMessage.signal();
	}
	
	void MessageSender::monitor()
	{
		std::stringstream ss;
		while ( true )
		{
			bool32 isDying = true;
			m_isDying.get(isDying);

			if ( isDying )
			{
				m_log.addMessage("MessageSender: closing thread.");
				break;
			}
			
			m_newMessage.wait();
						
			bool32 empty = m_queue.isEmpty();
			while ( !empty )
			{
				if ( isDying )
				{
					return;
				}
				
				uint32 cooldown = 0;
				std::string &message = m_queue.front();
				ss << "-> " << message;
				m_log.addMessage(ss.str());
				ss.str("");
				
				m_connection.send(message);
				m_queue.pop();
				m_cooldownTime.get(cooldown);
				timing::sleepMilliseconds(cooldown);
				
				empty = m_queue.isEmpty();
			}
		}
	}
	void MessageSender::log(const anp::dstring &message)
	{
		m_log.addMessage(message);
	}
}
}
