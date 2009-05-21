#ifndef _OUTPUTPROCESSOR_H_
#define _OUTPUTPROCESSOR_H_

#include <queue>
#include <string>
#include "../../multitaskinglib/activeobject.h"
#include "../../multitaskinglib/event.h"
#include "../../multitaskinglib/lock.h"
#include "frontend_interface.h"

struct Output {
	std::string str;
	int source;
};

class COutputProcessor: public CActiveObject {
public:
	COutputProcessor(OUTPUTHANDLER out);

	void initThread();
	void run();
	void flushThread();

	void addOutput(const std::string &str, int sourceID=-1);
	//void addOutput(const std::string &str); perhaps a bit overkill.. not needed for now

private:
	std::queue<Output> m_outputQueue;
	CEvent m_newOutputEvent;
	CMutex m_outputQueueMutex;
	OUTPUTHANDLER m_out;
};

COutputProcessor &operator<<(COutputProcessor &o, const std::string &str);
COutputProcessor &operator<<(COutputProcessor &o, const char *str);
COutputProcessor &operator<<(COutputProcessor &o, char ch);

#endif // _OUTPUTPROCESSOR_H_