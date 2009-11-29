#include <basedefs.h>
#include <sys/time.h>
#include <unistd.h>

namespace anp
{
namespace timing
{
	uint32 getMilliseconds()
	{
		timeval curr;
		
		gettimeofday(&curr, NULL);
		
		return curr.tv_sec*1000 + curr.tv_usec/1000;
	}
	
	void sleepMilliseconds(uint32 ms)
	{
		usleep(ms*1000);
	}
}
}
