#include<cstdlib>
#include <iostream>
#include "jobinterface.h"
#include <random>

using JobSystem::JobManager;
using JobSystem::JobType;
using JobSystem::JobHandle;
using JobSystem::INVALID_JOB_HANDLE;

std::mutex mutex;

std::thread::id mainThreadId;
static int mainJobCount{ 0 };
static int otherThreadJobCount{ 0 };
std::random_device rd;
std::default_random_engine engine(rd());
std::uniform_int_distribution<int> dist(0, 99999999);

void job_spawn(std::uint64_t currentJob, std::uint64_t jobCount)
{
	std::cout << "Running in thread:" << std::this_thread::get_id() << "current job:" << currentJob << ", jobCount" << jobCount << std::endl;

	JobHandle job = INVALID_JOB_HANDLE;
	bool foreground = true;
	if (currentJob % 2)
		foreground = false;
	bool isNextLayerJob{ false };
	if(currentJob < jobCount)
	{
		//job = JobManager::Instance().AllocateJob(type, INVALID_JOB_HANDLE, job_spawn, currentJob + 1, jobCount);
		if (foreground)
			job = NEW_QUICK_JOB(INVALID_JOB_HANDLE, job_spawn, currentJob + 1, jobCount);
		else
			job = NEW_SLOW_JOB(INVALID_JOB_HANDLE, job_spawn, currentJob + 1, jobCount);
	}
	else
	{
		isNextLayerJob = true;
		std::uint64_t nextLayerJobCount = jobCount == static_cast<std::uint64_t>(-1) ? jobCount : jobCount + 1;
		if (foreground)
			job = NEW_QUICK_JOB(INVALID_JOB_HANDLE, job_spawn, 0, nextLayerJobCount);
		else
			job = NEW_SLOW_JOB(INVALID_JOB_HANDLE, job_spawn, 0, nextLayerJobCount);
		JobManager::Instance().SetBackgroundWorkersCount(nextLayerJobCount % 5 + 1);
	}
	if (isNextLayerJob)
	{
		JOB_START(job);
		mainJobCount++;
	}
	else
	{
		JOB_START(job);
		otherThreadJobCount++;
	}
	
	auto newJobCount = dist(engine) % 100;
	for (int i = 0;i < newJobCount;++i)
	{
		if (foreground)
		{
			QUICK_JOB_RUN(SpawnExtraJob, []() {std::cout << "This is extra job!"; });
		}
		else
		{
			SLOW_JOB_RUN(SpawnExtraJob, []() {std::cout << "This is extra job!"; });
		}
	}
	int a = 0;
	auto loopCount = dist(engine);
	for (int i = 0;i < loopCount;++i)
	{
		a += i;
	}
}

void task_generation_job()
{
	std::vector<JobHandle> jobs;
	auto masterJob = NEW_QUICK_JOB(INVALID_JOB_HANDLE, []() {std::cout << "Master job created!" << std::endl; });
	jobs.push_back(masterJob);
	for (int i = 0;i < 2;i++)
	{
		auto job = NEW_QUICK_JOB(masterJob, job_spawn, 0, 2);
		jobs.push_back(job);
	}
	for (auto job : jobs)
		JOB_START(job);
	JOB_WAIT(masterJob);
	std::cout << "All calculation job done!shutting up job system!" << std::endl;
	//JobManager::Instance().ShutDown();
	//std::cout << "JobSystem shut down!" << std::endl;
}

class TestClass
{
public:
	TestClass(int a):_a(a)
	{
		std::cout << "Constructor" << std::endl;
	}
	~TestClass()
	{
		std::cout << "Test Class destruct" << std::endl;
	}
	//TestClass(const TestClass& tc) :_a(tc._a)
	//{
	//	std::cout << "Copy constructor" << std::endl;
	//}
	//TestClass(TestClass&& tc) :_a(tc._a)
	//{
	//	std::cout << "Move constructor" << std::endl;
	//}
	void Print()const
	{
		std::cout << "thread_id" << std::this_thread::get_id() << "a is " << _a << std::endl;
	}
private:
	int _a;
};

//std::function<void()> func;

class CallableObject
{
public:
	CallableObject(const TestClass& tc):mTC(tc)
	{
		std::cout << "CallableObject constructor" << std::endl;
	}
	CallableObject(const CallableObject& co) :mTC(co.mTC)
	{
		std::cout << "CallableObject copy constructor" << std::endl;
	}
	CallableObject(CallableObject&& co) :mTC(std::move(co.mTC))
	{
		std::cout << "CallableObject move constructor" << std::endl;
	}
	~CallableObject()
	{
		std::cout << "CallableObject destruct" << std::endl;
	}
	void operator()()
	{
		mTC.Print();
	}
private:
	TestClass mTC;
};

void hello()
{
	//auto masterJob = JobManager::Instance().AllocateJob(JobType::JOB_TYPE_FOREGROUND, INVALID_JOB_HANDLE, []() {});
	auto masterJob = NEW_QUICK_JOB(INVALID_JOB_HANDLE, []() {});
	std::vector<JobHandle> jobs;
	jobs.push_back(masterJob);
	for (std::size_t i = 0;i < 2;++i)
	{
		auto job = NEW_QUICK_JOB(masterJob, task_generation_job);
		jobs.push_back(job);
	}
	for (auto job : jobs)
		JOB_START(job);
	JOB_WAIT(masterJob);

}


void calc_sum(std::uint64_t start, std::uint64_t end, std::uint64_t* result)
{
	if (end == start)
	{
		*result = end;
	}
	else if (end - start == 1)
	{
		*result = start + end;
	}
	else
	{
		auto mid = (start + end) / 2;
		std::uint64_t* halfResult = new std::uint64_t[2];
		auto job1 = JobManager::Instance().AllocateJob(JobType::JOB_TYPE_FOREGROUND, INVALID_JOB_HANDLE, calc_sum, start, mid, &halfResult[0]);
		auto job2 = JobManager::Instance().AllocateJob(JobType::JOB_TYPE_FOREGROUND, INVALID_JOB_HANDLE, calc_sum, mid + 1 > end ? end : mid + 1, end, &halfResult[1]);
		JobManager::Instance().RunJob(job1);
		JobManager::Instance().RunJob(job2);
		JobManager::Instance().WaitForCompletion(job1);
		JobManager::Instance().WaitForCompletion(job2);
		*result = halfResult[0] + halfResult[1];
		delete[] halfResult;
	}
}

void start_calc_sum()
{
	std::uint64_t end{ 19999 };
	std::cout << "start to calculate sum from 1 to " << end << std::endl;
	std::uint64_t* result = new std::uint64_t;
	auto job = JobManager::Instance().AllocateJob(JobType::JOB_TYPE_FOREGROUND, INVALID_JOB_HANDLE, calc_sum, 1, end, result);
	JobManager::Instance().RunJob(job);
	JobManager::Instance().WaitForCompletion(job);
	std::cout << "The result of the sum is " << *result << std::endl;
	std::cout << "Finish all jobs.shutting down job system" << std::endl;
	JobManager::Instance().ShutDown();
}

int main(int argc, char** argv)
{
	mainThreadId = std::this_thread::get_id();
	JobManager::Instance().Run(hello, 8192);
	//JobManager::Instance().Run(start_calc_sum, 8192);
	/*
	constexpr int max_int = 99999999;
	std::atomic<int> ai(0);
	int i = 0;
	auto time_before = std::chrono::high_resolution_clock::now();
	for (; i < max_int; ++i);
	auto time_after = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(time_after - time_before);
	std::cout << "Time increment normal int is " << duration.count() << std::endl;

	time_before = std::chrono::high_resolution_clock::now();
	for (; ai < max_int;ai.fetch_add(1, std::memory_order_relaxed));
	time_after = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::milliseconds>(time_after - time_before);
	std::cout << "Time increment atomic relaxed int is " << duration.count() << std::endl;

	ai = 0;
	time_before = std::chrono::high_resolution_clock::now();
	for (; ai < max_int;ai.fetch_add(1, std::memory_order_seq_cst));
	time_after = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::milliseconds>(time_after - time_before);
	std::cout << "Time increment atomic sequence int is " << duration.count() << std::endl;
	*/
	system("pause");
	return 0;
}