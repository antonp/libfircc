/**
 * @file anp_workerthread.h
 * Defines the WorkerThread class.
 */

#ifndef _ANP_WORKERTHREAD_H_
#define _ANP_WORKERTHREAD_H_

#include <basedefs.h>
#include <anp_threading.h>

namespace anp
{
namespace threading
{
	class JobQueueWorkerInterface;
	
	class WorkerThread
	{
	public:
		WorkerThread();
		~WorkerThread();
		
		void start(JobQueueWorkerInterface *jobQueue);
		void stop();
		void join();
	private:
		Thread m_thread;
		JobQueueWorkerInterface *m_jobQueue;
		uint32 m_dying;
		Mutex m_dyingMutex;
		
		static void *threadEntry(void *arg);
		void loop();
	};
	
} // namespace threading
} // namespace anp

#endif // _ANP_WORKERTHREAD_H_
