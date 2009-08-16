/**
 * @file anp_workerthread.h
 * Defines the WorkerThread class.
 */

#include <basedefs.h>
#include <anp_threading.h>

namespace anp
{
namespace threading
{
	
	class WorkerThread
	{
	public:
		WorkerThread();
		~WorkerThread();
		
		void loop();
	private:
		Thread m_thread;
		
		static void *threadEntry(void *arg);
	};
	
} // namespace threading
} // namespace anp
