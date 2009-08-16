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
	
	Thread::Thread()
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
} // namespace threading
} // namespace anp
