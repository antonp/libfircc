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
		Result send(const std::string &buffer);
		Result receive(int8 *buffer, uint32 bufferSize);
	private:
		void connect(const std::string &hostname,
					 const std::string &port);
		void clean();
		int m_socket;
	};
} // namespace firc
} // namespace anp
#endif
