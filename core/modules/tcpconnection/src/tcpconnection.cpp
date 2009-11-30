#include "../tcpconnection.h"

#include <string.h> // for memset
#include <netdb.h>
#include <stdexcept>

//#pragma comment(lib, "wsock32.lib")
namespace anp
{
namespace firc
{
	TCPConnection::TCPConnection(
		const std::string &hostname,
		const std::string &port
	)
	{
		connect(hostname, port);
	}

	TCPConnection::~TCPConnection(void)
	{
		::close(m_socket);
		clean();
	}

	void TCPConnection::clean()
	{
		//WSACleanup();
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
				// Failed to connect
				throw NetworkException("Failed to connect");
			} else
			{
				// Successfully connected
			}
		}
	}

	Result TCPConnection::send(const std::string &buffer)
	{
		/// @todo Error-check return value
		::send(m_socket, buffer.c_str(), (int)buffer.length(), 0);
		return RES_OK;
	}

	Result TCPConnection::receive(int8 *buffer, uint32 bufferSize)
	{
		memset((void *)buffer, 0, bufferSize);
		if ( 0 >= ::recv(m_socket, buffer, bufferSize, 0) )
		{
			throw NetworkException("Failed to recv(), connection closed");
		}
		return RES_OK;
	}
} // namespace firc
} // namespace anp
