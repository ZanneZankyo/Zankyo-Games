#pragma once
#if !defined( __JOB__ )
#define __JOB__

#include "EngineBase.hpp"

#include "ThreadSafeQueue.hpp"
#include "Signal.hpp"
#include "Atomic.hpp"
#include "Thread.hpp"

enum eJobType : char
{
	JOB_GENERIC = 0,
	JOB_MAIN,
	JOB_IO,
	JOB_RENDER,

	JOB_TYPE_COUNT,
};

typedef void(*job_work_cb)(void*);

class Job
{
public:
	eJobType type;
	job_work_cb work_cb;

	void *user_data;

	std::vector<Job*> dependents;
	uint num_dependencies;

public:
	void on_finish();
	void on_dependancy_finished();

	void dependent_on(Job *parent);
	void add_dependent(Job *child);
};

class JobConsumer
{
public:
	std::vector<eJobType> types;
	//ThreadSafeQueue<Job*> jobQueue;

public:
	void add_category(eJobType category);

	// consumes a single job.  Returns true if a job
	// was consume, false if not job was ready.
	bool consume_job();

	// consumes as many jobs are as available
	// and returns the number consumed.
	uint consume_all();

};


class JobSystem
{
public:
	ThreadSafeQueue<Job*> *queues;
	Signal **signals;
	uint queue_count;

	bool is_running;

	JobConsumer *generic_consumer;
	JobConsumer *main_consumer;
	JobConsumer *io_consumer;
	JobConsumer *render_consumer;

	static JobSystem* s_instance;

public:

	static void Start(uint job_category_count = JOB_TYPE_COUNT, int generic_thread_count = -1);
};

void JobDispatchAndRelease(Job *job);
void JobSystemShutdown();

Job* JobCreate(eJobType type, job_work_cb work_cb, void* user_data);
void JobDispatch(Job *job);
void JobRelease(Job *job);
void JobDispatchAndRelease(Job *job);

//------------------------------------------------------------------------
// THIS SHOULD BE MOVED TO A JOB CONSUMER OBJECT!
uint JobConsumeAll(eJobType type);

void JobClear(eJobType type);
//------------------------------------------------------------------------
template <typename CB, typename TUPLE, size_t ...INDICES>
void ForwardJobArgumentsWithIndices(CB cb, TUPLE &args, std::integer_sequence<size_t, INDICES...>)
{
	cb(std::get<INDICES>(args)...);
}

//------------------------------------------------------------------------
template <typename CB, typename ...ARGS>
void ForwardArgumentsJob(void *ptr)
{
	pass_data_t<CB, ARGS...> *args = (pass_data_t<CB, ARGS...>*) ptr;
	ForwardJobArgumentsWithIndices(args->cb, args->args, std::make_index_sequence<sizeof...(ARGS)>());
	delete args;
}
//------------------------------------------------------------------------
template <typename CB, typename ...ARGS>
Job* JobCreate(eJobType type, CB work_cb, ARGS ...user_data)
{
	pass_data_t<CB, ARGS...> *pass = new pass_data_t<CB, ARGS...>(work_cb, user_data...);
	return JobCreate(type, ForwardArgumentsJob<CB, ARGS...>, (void*)pass);
}
//------------------------------------------------------------------------
template <typename CB, typename ...ARGS>
Job* JobCreate(CB work_cb, ARGS ...user_data)
{
	pass_data_t<CB, ARGS...> *pass = new pass_data_t<CB, ARGS...>(work_cb, user_data...);
	return JobCreate(JOB_GENERIC, ForwardArgumentsJob<CB, ARGS...>, (void*)pass);
}

template <typename CB, typename ...ARGS>
void JobRun(CB work_cb, ARGS ...user_data)
{
	Job* job = JobCreate(JOB_GENERIC, work_cb, user_data...);
	JobDispatchAndRelease(job);
}

template <typename CB, typename ...ARGS>
void JobRun(eJobType type, CB work_cb, ARGS ...user_data)
{
	Job* job = JobCreate(type, work_cb, user_data...);
	JobDispatchAndRelease(job);
}



#endif 