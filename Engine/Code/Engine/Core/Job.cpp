#include "Job.hpp"
#include "Thread.hpp"
#include "Atomic.hpp"

JobSystem *JobSystem::s_instance = nullptr;


//------------------------------------------------------------------------
static void GenericJobThread(Signal *signal)
{
	ThreadSetNameInVisualStudio("Job Thread: Generic");
	while (JobSystem::s_instance->is_running) {
		signal->wait();
		JobSystem::s_instance->generic_consumer->consume_all();
		//JobConsumeAll(JOB_GENERIC);
	}
	JobSystem::s_instance->generic_consumer->consume_all();
	//JobConsumeAll(JOB_GENERIC);
}

static void MainJobThread(Signal* signal)
{
	ThreadSetNameInVisualStudio("Job Thread: Main");
	while (JobSystem::s_instance->is_running) {
		signal->wait();
		JobSystem::s_instance->main_consumer->consume_all();
		//JobConsumeAll(JOB_MAIN);
	}
	JobSystem::s_instance->main_consumer->consume_all();
	//JobConsumeAll(JOB_MAIN);
}

static void RenderJobThread(Signal* signal)
{
	ThreadSetNameInVisualStudio("Job Thread: Render");
	while (JobSystem::s_instance->is_running) {
		signal->wait();
		JobSystem::s_instance->render_consumer->consume_all();
		//JobConsumeAll(JOB_RENDER);
	}
	JobSystem::s_instance->render_consumer->consume_all();
	//JobConsumeAll(JOB_RENDER);
}

static void IOJobThread(Signal* signal)
{
	ThreadSetNameInVisualStudio("Job Thread: IO");
	while (JobSystem::s_instance->is_running) {
		signal->wait();
		JobSystem::s_instance->io_consumer->consume_all();
		//JobConsumeAll(JOB_IO);
	}
	JobSystem::s_instance->io_consumer->consume_all();
	//JobConsumeAll(JOB_IO);
}

//------------------------------------------------------------------------
void Job::on_finish()
{
	for (uint i = 0; i < dependents.size(); ++i) {
		dependents[i]->on_dependancy_finished();
	}
}

//------------------------------------------------------------------------
void Job::on_dependancy_finished()
{
	AtomicDecrement(&num_dependencies);
	JobDispatchAndRelease(this);
}

//------------------------------------------------------------------------
void Job::dependent_on(Job *parent)
{
	AtomicIncrement(&num_dependencies);
	parent->dependents.push_back(this);
}

void Job::add_dependent(Job *child)
{
	AtomicIncrement(&(child->num_dependencies));
	dependents.push_back(child);
}

void JobSystem::Start(uint job_category_count /*= JOB_TYPE_COUNT*/, int generic_thread_count /*= -1*/)
{
	int core_count = (int)std::thread::hardware_concurrency();
	if (generic_thread_count <= 0) {
		core_count += generic_thread_count;
	}
	core_count--; // one is always being created - so subtract from total wanted;

				  // We need queues! 
	JobSystem::s_instance = new JobSystem();
	JobSystem::s_instance->queues = new ThreadSafeQueue<Job*>[job_category_count];
	JobSystem::s_instance->signals = new Signal*[job_category_count];
	JobSystem::s_instance->queue_count = job_category_count;
	JobSystem::s_instance->is_running = true;

	for (uint i = 0; i < job_category_count; ++i) {
		JobSystem::s_instance->signals[i] = nullptr;
	}

	// create the signal

	JobSystem::s_instance->generic_consumer = new JobConsumer();
	JobSystem::s_instance->generic_consumer->add_category(JOB_GENERIC);
	JobSystem::s_instance->signals[JOB_GENERIC] = new Signal();
	ThreadCreate(GenericJobThread, JobSystem::s_instance->signals[JOB_GENERIC]);

	JobSystem::s_instance->main_consumer = new JobConsumer();
	JobSystem::s_instance->main_consumer->add_category(JOB_MAIN);
	JobSystem::s_instance->signals[JOB_MAIN] = new Signal();
	ThreadCreate(MainJobThread, JobSystem::s_instance->signals[JOB_MAIN]);

	JobSystem::s_instance->io_consumer = new JobConsumer();
	JobSystem::s_instance->io_consumer->add_category(JOB_IO);
	JobSystem::s_instance->signals[JOB_IO] = new Signal();
	ThreadCreate(IOJobThread, JobSystem::s_instance->signals[JOB_IO]);

	JobSystem::s_instance->render_consumer = new JobConsumer();
	JobSystem::s_instance->render_consumer->add_category(JOB_RENDER);
	JobSystem::s_instance->signals[JOB_RENDER] = new Signal();
	ThreadCreate(RenderJobThread, JobSystem::s_instance->signals[JOB_RENDER]);

	for (int i = JOB_TYPE_COUNT - 1; i < core_count; ++i) {
		ThreadCreate(GenericJobThread, JobSystem::s_instance->signals[JOB_GENERIC]);
	}
}


//------------------------------------------------------------------------
void JobSystemShutdown()
{
	// dont' forget to clean up.
	// TODO!
	JobSystem::s_instance->is_running = false;
}


//------------------------------------------------------------------------
Job* JobCreate(eJobType type, job_work_cb work_cb, void* user_data)
{
	Job *job = new Job();
	job->type = type;
	job->work_cb = work_cb;
	job->user_data = user_data;
	job->num_dependencies = 1;

	return job;
}

void JobDispatch(Job *job)
{
	UNUSED(job);
}

void JobRelease(Job *job)
{
	UNUSED(job);
}

//------------------------------------------------------------------------
void JobDispatchAndRelease(Job *job)
{
	// if I'm not ready to run, don't. 
	uint dcount = AtomicDecrement(&job->num_dependencies);
	if (dcount != 0) {
		return;
	}


	JobSystem::s_instance->queues[job->type].push(job);
	Signal *signal = JobSystem::s_instance->signals[job->type];
	if (signal) {
		signal->signal_all();
	}
}

//------------------------------------------------------------------------
// THIS SHOULD BE MOVED TO A JOB CONSUMER OBJECT!
uint JobConsumeAll(eJobType type)
{
	Job *job;
	uint processed_jobs = 0;

	ThreadSafeQueue<Job*> &queue = JobSystem::s_instance->queues[type];
	while (queue.pop(&job)) {
		job->work_cb(job->user_data);
		++processed_jobs;

		job->on_finish();
		delete job;
	}

	return processed_jobs;
}

void JobClear(eJobType type)
{
	JobSystem::s_instance->queues[type].clear();
}

void JobConsumer::add_category(eJobType category)
{
	if (find(types.begin(), types.end(), category) == types.end())
		types.push_back(category);
}

bool JobConsumer::consume_job()
{
	Job* job = nullptr;

	if (types.empty())
		return false;

	/*auto& jobQueue = JobSystem::s_instance->queues[types[0]];
	if (jobQueue.pop(&job)) {
		job->work_cb(job->user_data);
		job->on_finish();
		delete job;
		return true;
	}*/
	for (auto type : types)
	{
		auto& jobQueue = JobSystem::s_instance->queues[type];
		while (jobQueue.pop(&job)) {
			job->work_cb(job->user_data);
			job->on_finish();
			delete job;
			return true;
		}
	}
	return false;
}

uint JobConsumer::consume_all()
{
	Job* job = nullptr;
	uint processed_jobs = 0;
	
	for (auto type : types)
	{
		auto& jobQueue = JobSystem::s_instance->queues[type];
		while (jobQueue.pop(&job)) {
			job->work_cb(job->user_data);
			++processed_jobs;

			job->on_finish();
			delete job;
		}
	}
	

	return processed_jobs;
}
