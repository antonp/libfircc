/**
 * @file anp_jobqueue.cpp
 * Implementation of the job queue system.
 */

#include "../anp_jobqueue.h"

namespace anp
{
namespace threading
{
	
	Job::Job():
	m_status(JOBSTATUS_CREATED)
	{
		
	}
	
	Job::~Job()
	{
		
	}
	
	Job::JobStatus Job::getStatus() const
	{
		return m_status;
	}
	
	
	JobQueue::JobQueue()
	{
		
	}

	JobQueue::~JobQueue()
	{
		
	}
	
} // namespace threading
} // namespace anp
