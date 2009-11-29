#include "../inc/messagesender.h"

/// @todo REMOVE IOSTREAM INCLUDE! REPLACE WITH INTERNAL LOGGING OR
/// SOMETHING
#include <iostream>
#include <stdexcept>

namespace anp
{
namespace firc
{

	static void *threadRunMessageSender(void *arg)
	{
		MessageSender *ms = (MessageSender *)arg;
		if ( NULL == ms )
		{
			throw std::invalid_argument("'Invalid MessageSender'");
		}
		ms->run();
		std::cout << "MessageSender: closing thread." << std::endl;
		pthread_exit(0);
	}
	
}
}
