/**
 * @file anp_jobqueue.h
 * Provides an api to create jobs or tasks and add them to a queue.
 * A number of worker threads will then work their way through the
 * queue.
 */

#ifndef _ANP_JOBQUEUE_H_
#define _ANP_JOBQUEUE_H_

#include <basedefs.h>
#include <queue>

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
		~Job();
		
		enum JobStatus
		{
			JOBSTATUS_CREATED,
			JOBSTATUS_QUEUED,
			JOBSTATUS_BEING_EXECUTED
		};
		
		JobStatus getStatus() const;
	protected:
		JobStatus m_status;
	};

	class JobQueue
	{
	public:
		JobQueue();
		~JobQueue();
	private:
		std::queue<Job *> m_queue;
	};

} // namespace threading
} // namespace anp

#endif // _ANP_JOBQUEUE_H_
