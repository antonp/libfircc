#ifndef _ANP_THREADING_H_
#define _ANP_THREADING_H_

/**
 * @file anp_threading.h
 * Provides a generic interface for working with threads.
 * A platform specific implementation must be provided/linked in.
 * @todo Create wrapper object calling createThreadObject in constructor
 * and destroyThreadObject in destructor.
 */

#include <basedefs.h>

namespace anp
{
namespace threading
{
	/**
	 * @brief
	 * The Thread object represents a single thread and can be used
	 * to carry out operations on that thread.
	 */
	struct ThreadPlatformSpecific;
	
	/**
	 * @brief
	 * The Mutex object can be used for synchronizing threads.
	 */
	struct Mutex;
	
	/**
	 * @brief
	 * Attributes and creation options for a thread.
	 */
	struct ThreadAttributes;

	/**
	 * @brief
	 * This class wraps the platform specific thread handle/identifier.
	 * It can be created on the stack!
	 */
	class Thread
	{
	public:
		Thread();
		~Thread();
		
		/**
		 * @brief
		 * Creates a thread.
		 * 
		 * @param[in] attr
		 * Attributes and creation options of the thread.
		 * 
		 * @param[in] startRoutine
		 * Entry function for the thread.
		 * 
		 * @param[in] arg
		 * Argument to be passed to the threads startRoutine.
		 * 
		 * @return
		 * RES_OK on success, specific error code on failure.
		 */
		Result create(const ThreadAttributes *attr,
			void *(*startRoutine)(void *), void *arg);
			
		/**
		 * @brief
		 * Wait for thread to finish. Exactly like pthread_join.
		 * 
		 * @param thread
		 * The thread to wait for.
		 * 
		 * @param valuePtr
		 * No clue.
		 * 
		 * @return
		 * RES_OK on success, specific error code on failure.
		 */
		Result join(void **valuePtr);
		
	private:
		ThreadPlatformSpecific *m_thread;
	};
		
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
	Result createMutexObject(Mutex **mutex);
	
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
	void destroyMutexObject(Mutex *mutex);
	
	/**
	 * @brief
	 * Locks the mutex.
	 * 
	 * @param[in] mutex
	 * The mutex to lock.
	 * 
	 * @return
	 * RES_OK on success, error code otherwise.
	 */
	Result mutexLock(Mutex *mutex);

	/**
	 * @brief
	 * Unlocks the mutex.
	 * 
	 * @param[in] mutex
	 * The mutex to unlock.
	 * 
	 * @return
	 * RES_OK on success, error code otherwise.
	 */	
	Result mutexUnlock(Mutex *mutex);
		
} // namespace threading
} // namespace anp

#endif // _ANP_THREADING_H_
