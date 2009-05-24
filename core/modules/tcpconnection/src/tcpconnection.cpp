#include "../tcpconnection.h"

#include <string.h> // for memset
#include <netdb.h>

//#pragma comment(lib, "wsock32.lib")
namespace firc{
	TCPConnection::TCPConnection()
	{
		
	}
	TCPConnection::TCPConnection(
		const std::string &hostname,
		const std::string &port
	)
	{
		// Nothing is done with return value..?
		connect(hostname, port);
	}

	TCPConnection::~TCPConnection(void)
	{
	}

	void TCPConnection::clean()
	{
		//WSACleanup();
	}

	Result TCPConnection::connect(
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
				return RES_FAILED;
			}
			
			if ( -1 == ::connect(	m_socket,
									pServInfoCurrent->ai_addr,
									pServInfoCurrent->ai_addrlen) )
			{
				// Failed to connect
				return RES_FAILED;
			} else
			{
				// Successfully connected
			}
		}
		
		return RES_OK;
	}

	void TCPConnection::closeSocket()
	{
	/*	closesocket(sock);*/
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
			return RES_FAILED;
		}
		return RES_OK;
	}

	int TCPConnection::getLastError()
	{
	/*	return lastError;*/
		return 0; /// @todo remove this function?
	}
}
