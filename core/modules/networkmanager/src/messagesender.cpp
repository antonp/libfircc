#include "../inc/messagesender.h"

#include <anp_timing.h>
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
		ms->monitor();
		std::cout << "MessageSender: closing thread." << std::endl;
		pthread_exit(0);
	}
	
	void MessageSender::addMessage(const std::string &message)
	{
		m_queue.push(message);
	}

	void MessageSender::setCooldownTime(uint32 ms)
	{
		m_cooldownTime.set(ms);
	}
	
	void MessageSender::monitor()
	{
		while ( true )
		{
			
						
			bool32 empty = m_queue.isEmpty();
			while ( !empty )
			{
				uint32 cooldown = 0;
				std::string &message = m_queue.front();
				
				m_connection.send(message);
				m_queue.pop();
				m_cooldownTime.get(cooldown);
				timing::sleepMilliseconds(cooldown);
			}
		}
	}
}
}
