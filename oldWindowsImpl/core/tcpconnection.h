#ifndef _TCP_CONNECTION_H_
#define _TCP_CONNECTION_H_

// Network class written by Anton Petersson :-)

#include <windows.h>
#include <winsock.h>
#include <cstdio>
#include <string>

#define MAX_HOST_LENGTH 256
#define MAX_DATA_LENGTH 2048
#define N_ERROR_HOSTNAME_TOO_LONG -1
#define N_ERROR_INVALID_HOST -2
#define N_ERROR_SOCKET_INVALID -3
#define N_ERROR_PORT_NOT_SET -4
#define N_ERROR_CONNECTION_FAILED -5
#define N_CONNECTION_CLOSED -6
#define N_OK 1

class CTCPConnection
{
public:
	CTCPConnection(const std::string &host, unsigned short p);
	virtual ~CTCPConnection(void);
	void setHostName(const std::string &host);
	void setPort(unsigned short p);
	int Connect();
	int Send(const std::string &buffer);
	int Receive(char *_pBuffer);
	void CloseSocket();
	void Clean();
	int getLastError();
private:
	WORD sockVersion;
	WSADATA wsaData;
	LPHOSTENT HostEntry;
	SOCKET sock;
	SOCKADDR_IN ServerInfo;
	int lastError;
};
#endif
