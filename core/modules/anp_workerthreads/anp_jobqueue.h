/**
 * @file anp_jobqueue.h
 * Provides an api to create jobs or tasks and add them to a queue.
 * A number of worker threads will then work their way through the
 * queue.
 */

#ifndef _ANP_JOBQUEUE_H_
#define _ANP_JOBQUEUE_H_

#include <basedefs.h>
#include <anp_threading.h>
#include <anp_workerthread.h> // forward declare when dynamic amount
#include <queue> // create a struct for it and forward declare it to
				// avoid including queue here...

namespace anp
{
namespace threading
{
	/**
	 * @brief
	 * Job base class.
	 */
	class Job
	{
	public:
		Job();
		virtual ~Job();
		
		void execute();
		
		enum JobStatus
		{
			JOBSTATUS_CREATED,
			JOBSTATUS_QUEUED,
			JOBSTATUS_EXECUTING
		};
		
		JobStatus getStatus() const;
	protected:
		JobStatus m_status;
		
		virtual void executeCustom() = 0;
	};

	class JobQueueOperatorInterface
	{
	public:
		virtual void start() = 0;
		virtual void stopWait() = 0;
		virtual void waitForJobsToBeCompleted() = 0;
		virtual void addJob(Job *job) = 0;
	};
	
	class JobQueueWorkerInterface
	{
	public:
		virtual bool32 waitForJob(Job **job) = 0;
	};

	class JobQueue:
		public JobQueueOperatorInterface,
		public JobQueueWorkerInterface
	{
	public:
		JobQueue();
		~JobQueue();
		
		void start();
		void stopWait();
		void waitForJobsToBeCompleted();
		bool32 waitForJob(Job **job);
		void addJob(Job *job);
	private:
		std::queue<Job *> m_queue;
		Mutex m_jobQueueMutex;
		Event m_newJobEvent;
		Event m_jobsCompleted;
		uint32 m_dying;
		Mutex m_dyingMutex;
		
		enum
		{
			MAX_THREADS=1
		};
		/// @todo make dynamic amount, changeable at runtime
		WorkerThread m_workers[MAX_THREADS];
	};

} // namespace threading
} // namespace anp

#endif // _ANP_JOBQUEUE_H_
