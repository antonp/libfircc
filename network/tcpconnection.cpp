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

#include "inc/tcpconnection.h"

#include <string.h>
#include <netdb.h>
#include <stdexcept>
#include <sstream>

namespace anp
{

	class NetworkException: public std::runtime_error
	{
	public:
		NetworkException(const std::string &message):
		std::runtime_error(message) { }
	};

namespace irc
{
	TCPConnection::TCPConnection(
		const std::string &hostname,
		const std::string &port
	)
	{
		TCPConnection::connect(hostname, port);
	}

	TCPConnection::~TCPConnection(void)
	{
		::close(m_socket);
		clean();
	}

	void TCPConnection::clean()
	{
	}

	void TCPConnection::connect(
		const std::string &hostname,
		const std::string &port)
	{
		struct addrinfo hints;
		struct addrinfo *pServInfoFirst = NULL;
		struct addrinfo *pServInfoCurrent = NULL;
		memset((void *)&hints, 0, sizeof(hints));
		hints.ai_family = PF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM; // TCP
		
		getaddrinfo(hostname.c_str(), port.c_str(), &hints,
					&pServInfoFirst);
		
		if ( pServInfoFirst == NULL )
		{
			throw NetworkException("Failed to getaddrinfo()!");
		}
		
		for ( pServInfoCurrent=pServInfoFirst;
			  pServInfoCurrent != NULL;
			  pServInfoCurrent = pServInfoCurrent->ai_next )
		{
			m_socket = ::socket(pServInfoCurrent->ai_family,
								pServInfoCurrent->ai_socktype,
								pServInfoCurrent->ai_protocol);
			if ( -1 == m_socket )
			{
				// Failed
				throw NetworkException("Failed to create socket file descriptor");
			}
			
			if ( -1 == ::connect(	m_socket,
									pServInfoCurrent->ai_addr,
									pServInfoCurrent->ai_addrlen) )
			{
				continue;
			} else
			{
				// Success
				freeaddrinfo(pServInfoFirst);
				return;
			}
		}
		
		// Failed to connect
		freeaddrinfo(pServInfoFirst);
		throw NetworkException("Failed to connect");
	}

	void TCPConnection::send(const std::string &buffer)
	{
		if ( 0 >= ::send(m_socket, buffer.c_str(), (int)buffer.length(), 0) )
		{
			throw NetworkException("Failed to send()");
		}
	}

	void TCPConnection::receive(int8 *buffer, uint32 bufferSize)
	{
		memset((void *)buffer, 0, bufferSize);
		if ( 0 >= ::recv(m_socket, buffer, bufferSize, 0) )
		{
			throw NetworkException("Failed to recv(), connection closed");
		}
	}

	bool32 TCPConnection::waitForSocket(uint32 timeoutSeconds,
										uint32 timeoutMicroseconds)
	{
		timeval timeout;
		timeout.tv_sec = timeoutSeconds;
		timeout.tv_usec = timeoutMicroseconds;
		fd_set readFileDescriptorSet;
		FD_ZERO(&readFileDescriptorSet);
		FD_SET(m_socket, &readFileDescriptorSet);
		if ( (-1) == ::select(m_socket+1, &readFileDescriptorSet,
						NULL, NULL, &timeout) )
		{
			throw NetworkException("select() returned -1");
		}
		return FD_ISSET(m_socket, &readFileDescriptorSet);
	}

} // namespace irc
} // namespace anp

