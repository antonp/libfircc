#ifndef _ANP_THREADING_H_
#define _ANP_THREADING_H_

/**
 * @file anp_threading.h
 * Provides a generic interface for working with threads.
 * A platform specific implementation must be provided/linked in.
 */

#include <basedefs.h>

namespace firc // to be a subnamespace of anp, and this API should only be defined in anp namespace
// basedefs should use anp namespace... :'( find-replace-all ! :)
{
	/**
	 * @brief
	 * The Thread object represents a single thread and can be used
	 * to carry out operations on that thread.
	 */
	struct Thread;
	
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
	Result createThreadObject(Thread **thread);
	
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
	void destroyThreadObject(Thread *thread);
	
	/**
	 * @brief
	 * Creates a thread.
	 * 
	 * @param[out] thread
	 * Thread object representing the newly created thread.
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
	Result createThread(Thread **thread, const ThreadAttributes *attr,
		void *(*startRoutine)(void *), void *arg);
	
} // namespace firc

#endif // _ANP_THREADING_H_
