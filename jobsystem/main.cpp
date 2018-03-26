#include<cstdlib>
#include <iostream>
#include "jobmanager.h"

using JobSystem::JobManager;
using JobSystem::JobType;

std::mutex mutex;


void calc_sum(int a, int b)
{
	int c = a + b;
	std::cout << "Running in thread:" << std::this_thread::get_id() << " equation:" << a + " + " << b << " = " << c << std::endl;
}

void task_generation_job()
{
	std::vector<JobSystem::IJob*> jobs;
	auto masterJob = JobManager::Instance().AllocateJob(JobType::SHORT_TERM, nullptr, []() {std::cout << "Master job created!" << std::endl; });
	jobs.push_back(masterJob);
	for (int i = 0;i < 100;i++)
	{
		std::cout << "Add job " << (i + 1) << " calculating " << (2 * i) << " + " << ( 5 * i - 10) << std::endl;
		auto job = JobManager::Instance().AllocateJob(JobType::SHORT_TERM, masterJob, calc_sum, 2 * i, 5 * i - 10);
		jobs.push_back(job);
	}
	for (auto job : jobs)
		JobManager::Instance().RunJob(job);
	JobManager::Instance().WaitForCompletion(masterJob);
	std::cout << "All calculation job done!shutting up job system!" << std::endl;
	JobManager::Instance().ShutDown();
	std::cout << "JobSystem shut down!" << std::endl;
}

void hello()
{
	std::cout << "Job system initialized finished!" << std::endl;
	auto job = JobManager::Instance().AllocateJob(JobType::SHORT_TERM, nullptr, task_generation_job);
	JobManager::Instance().RunJob(job);
}

int main(int argc, char** argv)
{
	JobManager::Instance().Run(hello);
	system("pause");
	return 0;
}