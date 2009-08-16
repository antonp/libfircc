/**
 * @file anp_workerthread.cpp
 * Implementation of the WorkerThread.
 */

#include "../anp_workerthread.h"

namespace anp
{
namespace threading
{
	WorkerThread::WorkerThread()
	{
		m_thread.create(NULL, threadEntry, (void *)this);
	}
	
	WorkerThread::~WorkerThread()
	{
		
	}
	
	void WorkerThread::loop()
	{
		// while not exiting
			// Fetch/wait for job
			
			// Execute job
	}
	
	void *WorkerThread::threadEntry(void *arg)
	{
		((WorkerThread *)arg)->loop();
	}
	
} // namespace threading
} // namespace anp
