#include "../inc/messagesender.h"

#include <anp_timing.h>
#include <anp_threadsafequeue.h>
#include <tcpconnection.h>

/// @todo REMOVE IOSTREAM INCLUDE! REPLACE WITH INTERNAL LOGGING OR
/// SOMETHING
#include <iostream>
#include <stdexcept>

namespace anp
{
namespace firc
{

	// Should be static but can't due to friendship :P
	void *threadRunMessageSender(void *arg)
	{
		MessageSender *ms = (MessageSender *)arg;
		if ( NULL == ms )
		{
			throw std::invalid_argument("'Invalid MessageSender'");
		}
		try
		{
			ms->monitor();
			std::cout << "MessageSender: closing thread." << std::endl;
		} catch ( std::exception &e )
		{
			std::cout << "MessageSender: Exception occured: " 
				<< e.what()	<< std::endl;
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
		while ( true )
		{
			bool32 isDying = true;
			m_isDying.get(isDying);

			if ( isDying )
			{
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
				
				m_connection.send(message);
				m_queue.pop();
				m_cooldownTime.get(cooldown);
				timing::sleepMilliseconds(cooldown);
				
				empty = m_queue.isEmpty();
			}
		}
	}
}
}
