/*

Copyright (c) 2010, Anton Petersson < anton (at) anp.nu >
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <iostream>
#include <unistd.h> // for Sleep
#include <string.h>
#include <sys/time.h>

#include <basedefs.h>
#include <anp_threading.h>
#include <anp_workerthread.h>
#include <anp_jobqueue.h>

using namespace anp;
using namespace anp::threading;
using std::cout;
using std::endl;
using std::string;

Mutex g_executionCountMutex;
uint32 g_executionCount = 0;

void incrExecCount()
{
	g_executionCountMutex.lock();
	g_executionCount++;
	g_executionCountMutex.unlock();
}

uint32 getExecCount()
{
	uint32 ret = 0;
	g_executionCountMutex.lock();
	ret = g_executionCount;
	g_executionCountMutex.unlock();	
	
	return ret;
}

class IdentifiedJob: public Job
{
public:
	IdentifiedJob(uint32 id): Job(), m_id(id)
	{
//		cout << "IdentifiedJob #" << m_id << " being created." << endl;
	}
	virtual ~IdentifiedJob()
	{
//		cout << "IdentifiedJob #" << m_id << " being destroyed." << endl;
	}
protected:
	uint32 m_id;
};

class ArithmeticJob: public IdentifiedJob
{
public:
	ArithmeticJob(uint32 id): IdentifiedJob(id)
	{
		
	}
	virtual ~ArithmeticJob()
	{
		
	}
protected:
	enum
	{
		CALCULATIONS = 128000
	};

	void executeCustom()
	{
		uint32 result = 8971323 * m_id;
		float result2 = 78.4f;
		for ( uint32 i=1; i<CALCULATIONS; ++i )
		{
			result += ((result / i) * 314 % 12) % 1024;
			result2 = (((double)result) / result2) * 56.3f / 0.9f;
		}
		result = result % 1024;
	
		incrExecCount();	
//		cout << "Result: " << result << endl;
//		cout << "Result2: " << result2 << endl;
	}
};

class BlockingJob: public IdentifiedJob
{
public:
	BlockingJob(uint32 id): IdentifiedJob(id)
	{
	}
	virtual ~BlockingJob()
	{
	}
private:
	void executeCustom()
	{
		sleep(10);
		incrExecCount();
	}
};

const uint32 MAX_JOBS = 100003;

void operateJobQueue(JobQueueOperatorInterface &jobQueue)
{
	cout << "Adding jobs." << endl;
	for ( uint32 i=0; i<MAX_JOBS; ++i )
	{
		jobQueue.addJob(new ArithmeticJob(i));
	}
	
	jobQueue.addJob(new BlockingJob(MAX_JOBS));
	
	cout << "Waiting for jobqueue to finish up." << endl;
	jobQueue.waitForJobsToBeCompleted();
	jobQueue.stopWait();
}

int main(int argc, char *argv[])
{
	JobQueue jobQueue;
	timeval a;
	timeval b;
	
	jobQueue.start();
	
	gettimeofday(&a, 0);
	operateJobQueue(jobQueue);
	gettimeofday(&b, 0);
	
	//while ( getExecCount() < 2000 ); // busy wait
	
	cout << "Executed " << getExecCount() << " jobs." << endl;
	cout << "Exiting. Time elapsed: " << (b.tv_sec - a.tv_sec) << endl;
	return 0;
}
