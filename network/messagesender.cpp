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

#include "inc/messagesender.h"

#include <anpcode/anp_timing.h>
#include <anpcode/anp_threadsafequeue.h>
#include <anpcode/log_singleton.h>
#include "inc/tcpconnection.h"
#include <sstream>

#include <stdexcept>

namespace anp
{
namespace irc
{
	void *threadRunMessageSender(void *arg)
	{
		MessageSender *ms = (MessageSender *)arg;
		if ( NULL == ms )
		{
			//throw std::invalid_argument("'Invalid MessageSender'");
			try
			{
                ANPLOGE("libfirc", "Invalid MessageSender.");
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
                ANPLOGE("libfirc", std::string("Exception occured: ")+e.what());
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

	void MessageSender::setCooldownTime(unsigned int ms)
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
			bool isDying = true;
			m_isDying.get(isDying);

			if ( isDying )
			{
                ANPLOGI("libfirc", "closing thread.");
				break;
			}
			
			m_newMessage.wait();
						
			bool empty = m_queue.isEmpty();
			while ( !empty )
			{
				if ( isDying )
				{
					return;
				}
				
				unsigned int cooldown = 0;
				std::string &message = m_queue.front();
				ss << "-> " << message;
                ANPLOGI("libfirc", ss.str());
				ss.str("");
				
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
