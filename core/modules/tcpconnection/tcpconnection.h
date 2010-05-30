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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef _TCP_CONNECTION_H_
#define _TCP_CONNECTION_H_

#include <basedefs.h>
//#include <windows.h>
//#include <winsock.h>
#include <cstdio>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdexcept>

#define MAX_HOST_LENGTH 256
#define MAX_DATA_LENGTH 2048

namespace anp
{
	class NetworkException: public std::runtime_error
	{
	public:
		NetworkException(const std::string &message):
		std::runtime_error(message) { }
	};
	
namespace firc
{
	class TCPConnection
	{
	public:
		TCPConnection(const std::string &hostname,
						const std::string &port);
		virtual ~TCPConnection();
		void send(const std::string &buffer);
		void receive(int8 *buffer, uint32 bufferSize);
		bool32 waitForSocket(uint32 timeoutSeconds,
					uint32 timeoutMicroseconds);
	private:
		void connect(const std::string &hostname,
					 const std::string &port);
		void clean();
		int m_socket;
	};
} // namespace firc
} // namespace anp
#endif
