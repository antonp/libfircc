#include "outputprocessor.h"

COutputProcessor::COutputProcessor(OUTPUTHANDLER out):
m_out(out) {

	m_thread.resume();
}

void COutputProcessor::initThread() {

}

void COutputProcessor::run() {
	Output item;
	while ( !m_isDying ) {
		// Wait for output
		m_newOutputEvent.wait();

		// When there's output to process, go through the queue
		while ( 1 ) {
			// Reserve the outputQueue so that it can't be
			// manipulated while we're reading from it.
			// This is also great as it gives the other
			// threads a chance to add new output data
			// in between the reads
			{
				CLock lock(m_outputQueueMutex);
				if ( m_outputQueue.empty() ) {
					break;
				} else {
					// This solution requires that we copy the string
					// before we can send it to the output function
					// in the frontend. This is easily solved if we
					// only save pointers to the string obejcts in the
					// queue
					item = m_outputQueue.front();
					m_outputQueue.pop();
				
				}
			}
			// At this point the lock has gone out of
			// scope, so if the output function in the
			// frontend is slow/blocking, the output
			// queue can still grow
			m_out(item.str.c_str(), item.source);
		}
	}
}

void COutputProcessor::flushThread() {
	m_newOutputEvent.release();
}

void COutputProcessor::addOutput(const std::string &str, int sourceID) {
	// Reserve the outputQueue so that
	// no other thread can manipulate it
	// while we are adding our output data
	CLock lock(m_outputQueueMutex);
	Output item;
	item.str = str;
	item.source = sourceID;
	m_outputQueue.push(item);
	// Let the output processor now there
	// are new entries in the queue
	m_newOutputEvent.release();
}

COutputProcessor &operator<<(COutputProcessor &o, const std::string &str) {
	o.addOutput(str);
	return o;
}

COutputProcessor &operator<<(COutputProcessor &o, const char *str) {
	o.addOutput(str);
	return o;
}

COutputProcessor &operator<<(COutputProcessor &o, char ch) {
	std::string str;
	str = ch;
	o.addOutput(str);
	return o;
}