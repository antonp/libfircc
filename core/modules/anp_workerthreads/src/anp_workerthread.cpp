/**
 * @file anp_workerthread.cpp
 * Implementation of the WorkerThread.
 */

#include "../anp_workerthread.h"
#include <anp_jobqueue.h>

namespace anp
{
namespace threading
{
	WorkerThread::WorkerThread():
	m_dying(0)
	{
		
	}
	
	WorkerThread::~WorkerThread()
	{
		
	}
	
	void WorkerThread::start(JobQueueWorkerInterface *jobQueue)
	{
		m_jobQueue = jobQueue;
		m_thread.create(NULL, threadEntry, (void *)this);
	}
	
	void WorkerThread::stop()
	{
		/// @todo implement, force stop or raise a dying flag and wait?
		m_dyingMutex.lock();
		m_dying = 1;
		m_dyingMutex.unlock();
	}
	
	void WorkerThread::join()
	{
		m_thread.join(NULL);
	}
	
	void WorkerThread::loop()
	{
		Job *job = NULL;
		uint32 dying = 0;

		m_dyingMutex.lock();
		dying = m_dying;
		m_dyingMutex.unlock();

		while ( 0 == dying ) {
			if ( m_jobQueue->waitForJob(&job) == false || NULL == job )
			{
				// The job queue is shutting down
				break;
			}

			if ( NULL != job )
			{
				job->execute();
			}

			delete job;
			job = NULL;
			
			m_dyingMutex.lock();
			dying = m_dying;
			m_dyingMutex.unlock();
		}
	}
	
	void *WorkerThread::threadEntry(void *arg)
	{
		((WorkerThread *)arg)->loop();
	}
	
} // namespace threading
} // namespace anp
