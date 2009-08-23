/**
 * @file anp_threading_posix.cpp
 * Posix implementation of the anp threading API.
 */


#include <anp_threading.h>
#include <pthread.h>

#include <iostream> // ETODO REMOVE THIS SHIT

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
	
	struct EventPlatformSpecific
	{
		pthread_cond_t m_cond;
		pthread_mutex_t m_mutex;
	};
	
	///////////////
	// Functions //
	///////////////
	
	// Thread
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
	
	// Mutex
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
	
	// Event
	Result createEventObject(EventPlatformSpecific **event)
	{
		if ( NULL == event )
		{
			return RES_INVALID_PARAMETER;
		}
		*event = new EventPlatformSpecific;
		if ( NULL == *event )
		{
			return RES_MEMALLOC_FAILED;
		}
		pthread_cond_init(&((*event)->m_cond), NULL);
		pthread_mutex_init(&((*event)->m_mutex), NULL);
		return RES_OK;
	}
	
	void destroyEventObject(EventPlatformSpecific *event)
	{
		if ( NULL != event )
		{
			pthread_cond_destroy(&event->m_cond);
			pthread_mutex_destroy(&event->m_mutex);
			delete event;
		}
	}
	
	Result Event::wait()
	{
		Result res = RES_OK;
		
		pthread_mutex_lock(&m_event->m_mutex);
		pthread_cond_wait(&m_event->m_cond, &m_event->m_mutex);
		pthread_mutex_unlock(&m_event->m_mutex);
		
		return res;
	}
	
	Result Event::signal()
	{
		/// @todo error codes
		Result res = RES_OK;
		pthread_mutex_lock(&m_event->m_mutex);
		if ( 0 != pthread_cond_signal(&m_event->m_cond) )
		{
			/// @todo error
		}
		pthread_mutex_unlock(&m_event->m_mutex);
		
		return res;
	}
	
	Result Event::signalBroadcast()
	{
		Result res = RES_OK;
		
		pthread_mutex_lock(&m_event->m_mutex);
		pthread_cond_broadcast(&m_event->m_cond);
		pthread_mutex_unlock(&m_event->m_mutex);
		
		return res;
	}
	
} // namespace threading
} // namespace anp
