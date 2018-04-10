#include<cstdlib>
#include <iostream>
#include "jobmanager.h"
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
	JobType type = JobType::FOREGROUND;
	if (currentJob % 2)
		type = JobType::BACKGROUND;
	bool isNextLayerJob{ false };
	if(currentJob < jobCount)
	{
		job = JobManager::Instance().AllocateJob(type, INVALID_JOB_HANDLE, job_spawn, currentJob + 1, jobCount);
	}
	else
	{
		isNextLayerJob = true;
		std::uint64_t nextLayerJobCount = jobCount == static_cast<std::uint64_t>(-1) ? jobCount : jobCount + 1;
		job = JobManager::Instance().AllocateJob(type, INVALID_JOB_HANDLE, job_spawn, 0, nextLayerJobCount);
		JobManager::Instance().SetBackgroundWorkersCount(nextLayerJobCount % 5 + 1);
	}
	if (isNextLayerJob)
	{
		JobManager::Instance().RunJob(job);
		mainJobCount++;
	}
	else
	{
		JobManager::Instance().RunJob(job);
		otherThreadJobCount++;
	}
	auto newJobCount = dist(engine) % 100;
	for (int i = 0;i < newJobCount;++i)
	{
		job = JobManager::Instance().AllocateJob(type, INVALID_JOB_HANDLE, []() {std::cout << "This is extra job!"; });
		JobManager::Instance().RunJob(job);
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
	auto masterJob = JobManager::Instance().AllocateJob(JobType::FOREGROUND, INVALID_JOB_HANDLE, []() {std::cout << "Master job created!" << std::endl; });
	jobs.push_back(masterJob);
	for (int i = 0;i < 2;i++)
	{
		auto job = JobManager::Instance().AllocateJob(JobType::FOREGROUND, masterJob, job_spawn, 0, 2);
		jobs.push_back(job);
	}
	for (auto job : jobs)
		JobManager::Instance().RunJob(job);
	JobManager::Instance().WaitForCompletion(masterJob);
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
	CallableObject(const TestClass& tc):m_tc(tc)
	{
		std::cout << "CallableObject constructor" << std::endl;
	}
	CallableObject(const CallableObject& co) :m_tc(co.m_tc)
	{
		std::cout << "CallableObject copy constructor" << std::endl;
	}
	CallableObject(CallableObject&& co) :m_tc(std::move(co.m_tc))
	{
		std::cout << "CallableObject move constructor" << std::endl;
	}
	~CallableObject()
	{
		std::cout << "CallableObject destruct" << std::endl;
	}
	void operator()()
	{
		m_tc.Print();
	}
private:
	TestClass m_tc;
};

void hello()
{
	auto masterJob = JobManager::Instance().AllocateJob(JobType::FOREGROUND, INVALID_JOB_HANDLE, []() {});
	std::vector<JobHandle> jobs;
	jobs.push_back(masterJob);
	for (std::size_t i = 0;i < 2;++i)
	{
		auto job = JobManager::Instance().AllocateJob(JobType::FOREGROUND, masterJob, task_generation_job);
		jobs.push_back(job);
	}
	for (auto handle : jobs)
		JobManager::Instance().RunJob(handle);
	JobManager::Instance().WaitForCompletion(masterJob);

}

int main(int argc, char** argv)
{
	mainThreadId = std::this_thread::get_id();
	JobManager::Instance().Run(hello);
	system("pause");
	return 0;
}