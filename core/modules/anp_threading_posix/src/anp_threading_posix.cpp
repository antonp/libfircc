/**
 * @file anp_threading_posix.cpp
 * Posix implementation of the anp threading API.
 */


#include <anp_threading.h>
#include <pthread.h>

namespace firc
{
	////////////////
	// Data types //
	////////////////
	
	struct Thread
	{
		pthread_t m_thread;
	};
	
	struct Mutex
	{
		pthread_mutex_t m_mutex;
	};
	
	struct ThreadAttributes
	{
		pthread_attr_t m_attributes;
	};
	
	///////////////
	// Functions //
	///////////////
	
	Result createThreadObject(Thread **thread)
	{
		if ( NULL == thread )
		{
			return RES_INVALID_PARAMETER;
		}
		*thread = new Thread;
		if ( NULL == *thread )
		{
			return RES_MEMALLOC_FAILED;
		}
		return RES_OK;
	}

	void destroyThreadObject(Thread *thread)
	{
		delete thread;
	}
	
	Result createThread(Thread *thread, const ThreadAttributes *attr,
		void *(*startRoutine)(void *), void *arg)
	{
		/// @todo Add support for attr parameter
		Result res = RES_INVALID_PARAMETER;
		
		// attr and arg are allowed to be NULL
		if ( NULL != thread
			&& NULL != startRoutine )
		{
			if ( 0 == pthread_create(&thread->m_thread, NULL,
				startRoutine, arg) )
			{
				res = RES_OK;
			} else
			{
				res = RES_FAILED;
			}
		}
		return res;
	}
}
