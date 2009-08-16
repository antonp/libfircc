/**
 * @file anp_threading_posix.cpp
 * Posix implementation of the anp threading API.
 */


#include <anp_threading.h>
#include <pthread.h>

namespace anp
{
namespace threading
{
	////////////////
	// Data types //
	////////////////
	
	struct ThreadPlatformSpecific
	{
		pthread_t m_thread;
	};
	
	struct MutexPlatformSpecific
	{
		pthread_mutex_t m_mutex;
	};
	
	struct ThreadAttributesPlatformSpecific
	{
		pthread_attr_t m_attributes;
	};
	
	///////////////
	// Functions //
	///////////////
	
	Result createThreadObject(ThreadPlatformSpecific **thread)
	{
		if ( NULL == thread )
		{
			return RES_INVALID_PARAMETER;
		}
		*thread = new ThreadPlatformSpecific;
		if ( NULL == *thread )
		{
			return RES_MEMALLOC_FAILED;
		}
		return RES_OK;
	}

	void destroyThreadObject(ThreadPlatformSpecific *thread)
	{
		if ( NULL != thread )
		{
			delete thread;
		}
	}
	
	Result Thread::create(const ThreadAttributesPlatformSpecific *attr,
		void *(*startRoutine)(void *), void *arg)
	{
		/// @todo Add support for attr parameter
		Result res = RES_INVALID_PARAMETER;
		
		// attr and arg are allowed to be NULL
		if ( NULL != startRoutine )
		{
			if ( 0 == pthread_create(&m_thread->m_thread, NULL,
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
	
	Result Thread::join(void **valuePtr)
	{
		Result res = RES_INVALID_PARAMETER;
		
		res = (0 == pthread_join(m_thread->m_thread, valuePtr) ?
			RES_OK: RES_FAILED);

		return res;
	}
	
	Result createMutexObject(MutexPlatformSpecific **mutex)
	{
		if ( NULL == mutex )
		{
			return RES_INVALID_PARAMETER;
		}
		*mutex = new MutexPlatformSpecific;
		if ( NULL == *mutex )
		{
			return RES_MEMALLOC_FAILED;
		}
		pthread_mutex_init(&((*mutex)->m_mutex), NULL);
		return RES_OK;
	}

	void destroyMutexObject(MutexPlatformSpecific *mutex)
	{
		if ( NULL != mutex )
		{
			pthread_mutex_destroy(&mutex->m_mutex);
			delete mutex;
		}
	}
	
	Result Mutex::lock()
	{
		Result res = RES_INVALID_PARAMETER;

		res = (0 == pthread_mutex_lock(&m_mutex->m_mutex) ?
			RES_OK: RES_FAILED);

		return res;
	}

	Result Mutex::unlock()
	{
		Result res = RES_INVALID_PARAMETER;

		res = (0 == pthread_mutex_unlock(&m_mutex->m_mutex) ?
			RES_OK: RES_FAILED);

		return res;
	}
	
} // namespace threading
} // namespace anp
