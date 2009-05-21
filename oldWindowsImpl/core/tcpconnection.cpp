#include "tcpconnection.h"

#pragma comment(lib, "wsock32.lib")

CTCPConnection::CTCPConnection(const std::string &host, unsigned short p) {
	lastError = N_OK;
	sockVersion=MAKEWORD(1, 1);
	WSAStartup(sockVersion, &wsaData);
	
	// Host crap
	HostEntry=gethostbyname(host.c_str());
	if ( !HostEntry ) {
		lastError = N_ERROR_INVALID_HOST;
		Clean();
		return;
	}
	// Other init stuff
	sock = socket(AF_INET,		// Go over TCP/IP
				SOCK_STREAM,	// This is a stream-oriented socket
				IPPROTO_TCP);	// Use TCP rather than UDP
	if ( sock == INVALID_SOCKET ) {
		lastError=N_ERROR_SOCKET_INVALID;
		CloseSocket();
		Clean();
		return;
	}
	ServerInfo.sin_family = AF_INET;
	ServerInfo.sin_addr = *((LPIN_ADDR)*HostEntry->h_addr_list);
	ServerInfo.sin_port = htons(p);		// Change to network-byte order
	lastError = N_OK;
}

CTCPConnection::~CTCPConnection(void) {
}

void CTCPConnection::setHostName(const std::string &host) {
	HostEntry = gethostbyname(host.c_str());
	ServerInfo.sin_addr = *((LPIN_ADDR)*HostEntry->h_addr_list);
}

void CTCPConnection::setPort(unsigned short p) {
	ServerInfo.sin_port = htons(p);
}

void CTCPConnection::Clean() {
	WSACleanup();
}

int CTCPConnection::Connect() {
	// Connect to the server
	if ( connect(sock, (LPSOCKADDR)&ServerInfo, sizeof(struct sockaddr)) != 0 ) {
		lastError=N_ERROR_CONNECTION_FAILED;
		return N_ERROR_CONNECTION_FAILED;
	}
	return N_OK;
}

void CTCPConnection::CloseSocket() {
	closesocket(sock);
}

int CTCPConnection::Send(const std::string &buffer) {
	// Add some more error-checking later.
	send(sock, buffer.c_str(), (int)buffer.length(), 0);
	return N_OK;
}

int CTCPConnection::Receive(char *_pBuffer) {
	ZeroMemory(_pBuffer, MAX_DATA_LENGTH);
	if ( 0 >= recv(sock, _pBuffer, MAX_DATA_LENGTH, 0) )
		return N_CONNECTION_CLOSED;
	return N_OK;
}

int CTCPConnection::getLastError() {
	return lastError;
}
