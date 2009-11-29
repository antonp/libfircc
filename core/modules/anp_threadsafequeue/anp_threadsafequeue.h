#ifndef _ANP_THREADSAFEQUEUE_H_
#define _ANP_THREADSAFEQUEUE_H_

#include <basedefs.h>
#include <queue>

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
	
	template<typename T>
	struct InternalThreadSafeQueue
	{
		std::queue<T> m_queue;
	};

	template<typename T>
	ThreadSafeQueue<T>::ThreadSafeQueue()
	{
		m_queue = new InternalThreadSafeQueue<T>;
	}
	
	template<typename T>
	ThreadSafeQueue<T>::~ThreadSafeQueue()
	{
		delete m_queue;
	}

	template<typename T>
	void ThreadSafeQueue<T>::push(const T &element)
	{
		m_queue->m_queue.push(element);
	}
	
	template<typename T>
	T &ThreadSafeQueue<T>::front()
	{
		return m_queue->m_queue.front();
	}

	template<typename T>
	void ThreadSafeQueue<T>::pop()
	{
		m_queue->m_queue.pop();
	}
	
	template<typename T>
	uint32 ThreadSafeQueue<T>::getSize() const
	{
		return m_queue->m_queue.size();
	}
	
	template<typename T>
	bool32 ThreadSafeQueue<T>::isEmpty() const
	{
		return m_queue->m_queue.empty();
	}
}
}

#endif // _ANP_THREADSAFEQUEUE_H_
