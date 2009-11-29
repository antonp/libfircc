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
	 * The ThreadPlatformSpecific object represents a single
	 * thread and can be used to carry out operations on that thread.
	 */
	struct ThreadPlatformSpecific;
	
	/**
	 * @brief
	 * The MutexPlatformSpecific object can be used for
	 * synchronizing threads.
	 */
	struct MutexPlatformSpecific;
	
	/**
	 * @brief
	 * Attributes and creation options for a thread.
	 */
	struct ThreadAttributesPlatformSpecific;
	
	/**
	 * @brief
	 * The EventPlatformSpecific object represents a synchronization
	 * event. 
	 */
	struct EventPlatformSpecific;

	/**
	 * @brief
	 * This class wraps the platform specific thread handle/identifier.
	 * It can be created on the stack!
	 */
	class Thread
	{
	public:
		Thread();
		virtual ~Thread();
		
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
		Result create(const ThreadAttributesPlatformSpecific *attr,
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
	
	class Mutex
	{
	public:
		Mutex();
		~Mutex();
		
		/**
		 * @brief
		 * Locks the mutex.
		 * 
		 * @return
		 * RES_OK on success, error code otherwise.
		 */
		Result lock();

		/**
		 * @brief
		 * Unlocks the mutex.
		 * 
		 * @return
		 * RES_OK on success, error code otherwise.
		 */	
		Result unlock();
	private:
		MutexPlatformSpecific *m_mutex;
	};
	
	class Event
	{
	public:
		Event();
		~Event();
		
		Result wait();
		Result signal();
		Result signalBroadcast();
	private:
		EventPlatformSpecific *m_event;
	};
	
	template<typename T, T INIT>
	class ProtectedData
	{
	public:
		ProtectedData():m_data(INIT) { };
		
		void set(T value) {
			m_mutex.lock();
			m_data = value;
			m_mutex.unlock();
		}
		void get(T &value) const {
			m_mutex.lock();
			value = m_data;
			m_mutex.unlock();
		}
	protected:
		mutable Mutex m_mutex;
		T m_data;	
	};
} // namespace threading
} // namespace anp

#endif // _ANP_THREADING_H_
