#ifndef _NETWORKMANAGER_H_
#define _NETWORKMANAGER_H_

#include <basedefs.h>
#include <tcpconnection.h>

#include <pthread.h> // fix with johnny bigert article on interfaces

namespace firc
{
	class NetworkManager
	{
	public:
		NetworkManager();
		
		Result init();
		void deinit();
		enum State
		{
			CONNECTING=0,
			REGISTERING,
			CONNECTED,
			SHUTTING_DOWN,
			UNKNOWN
		};
		// Should be private (interface? friend with wrapper?)
		Result runMessageReceiver();
		
		Result sendMessage(const std::string &message);
	private:
	
		State m_state;
		pthread_mutex_t m_stateMutex;
		
		TCPConnection m_connection;
		
		pthread_t m_receiverThread;
	};
}

#endif // _NETWORKMANAGER_H_
