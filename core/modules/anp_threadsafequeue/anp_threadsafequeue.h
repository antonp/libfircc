#ifndef _ANP_THREADSAFEQUEUE_H_
#define _ANP_THREADSAFEQUEUE_H_

#include <basedefs.h>

namespace anp
{
namespace threading
{
	template<typename T>
	struct InternalThreadSafeQueue;
	
	template<typename T>
	class ThreadSafeQueue
	{
	public:
		ThreadSafeQueue();
		virtual ~ThreadSafeQueue();
	
		void push(const T &element);
		T &front();
		void pop();
		uint32 getSize() const;
		bool32 isEmpty() const;
	private:
		InternalThreadSafeQueue<T> *m_queue;
	};
}
}

#endif // _ANP_THREADSAFEQUEUE_H_
