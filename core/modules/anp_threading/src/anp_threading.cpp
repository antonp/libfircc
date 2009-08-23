/**
 * @file anp_threading.cpp
 * Implementation of generic parts of the threading api.
 */

#include "../anp_threading.h"
#include <basedefs.h>

namespace anp
{
namespace threading
{
	
	/**
	 * @brief
	 * Allocate memory and initialize a Thread object.
	 * 
	 * @param[out] thread
	 * The created Thread object.
	 * 
	 * @return
	 * RES_OK on success, an error code otherwise.
	 * 
	 * @remark
	 * Does @b NOT create a thread. Use createThread for that.
	 * 
	 * @sa
	 * destroyThreadObject
	 */
	Result createThreadObject(ThreadPlatformSpecific **thread);
	
	/**
	 * @brief
	 * Destroys the thread object.
	 * 
	 * @param[in] thread
	 * The thread object to destroy.
	 * 
	 * @sa
	 * createThreadObject
	 */
	void destroyThreadObject(ThreadPlatformSpecific *thread);
	
	Thread::Thread():
	m_thread(NULL)
	{
		if ( RES_OK != createThreadObject(&m_thread) )
		{
			/// @todo throw an exception here
			// throw anp::construction_failed exception!
		}
	}
	
	Thread::~Thread()
	{
		destroyThreadObject(m_thread);
	}
	
	/**
	 * @brief
	 * Allocate memory and initialize a Mutex object.
	 * 
	 * @param[out] mutex
	 * The created mutex object.
	 * 
	 * @return
	 * RES_OK on success, an error code otherwise.
	 * 
	 * @remark
	 * Does @b NOT create a thread. Use createThread for that.
	 * 
	 * @sa
	 * destroyMutexObject
	 */
	Result createMutexObject(MutexPlatformSpecific **mutex);
	
	/**
	 * @brief
	 * Destroys the Mutex object.
	 * 
	 * @param[in] mutex
	 * The mutex object to destroy.
	 * 
	 * @sa
	 * createMutexObject
	 */
	void destroyMutexObject(MutexPlatformSpecific *mutex);
	
	Mutex::Mutex():
	m_mutex(NULL)
	{
		if ( RES_OK != createMutexObject(&m_mutex) )
		{
			// exception? Might be thrown from createMutexObject though
		}
	}
	
	Mutex::~Mutex()
	{
		destroyMutexObject(m_mutex);
	}
	
	/**
	 * @brief
	 * Allocate memory for and initialize an Event object.
	 * 
	 * @param[out] event
	 * The created Event object.
	 * 
	 * @return
	 * RES_OK on success, an error code otherwise.
	 * 
	 * @sa
	 * destroyEventObject
	 */
	Result createEventObject(EventPlatformSpecific **event);
	
	/**
	 * @brief
	 * Destroys the Event object.
	 * 
	 * @param[in] event
	 * The event object to destroy.
	 * 
	 * @sa
	 * createEventObject
	 */
	void destroyEventObject(EventPlatformSpecific *event);

	Event::Event():
	m_event(NULL)
	{
		if ( RES_OK != createEventObject(&m_event) )
		{
			// exception? might be thrown from createEventObject though
		}
	}
	
	Event::~Event()
	{
		destroyEventObject(m_event);
	}
} // namespace threading
} // namespace anp
