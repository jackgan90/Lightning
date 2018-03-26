#include<cstdlib>
#include <iostream>
#include "jobmanager.h"

using JobSystem::JobManager;
using JobSystem::JobType;
using JobSystem::IJob;

std::mutex mutex;


void job_spawn(std::uint64_t currentJob, std::uint64_t jobCount)
{
	std::cout << "Running in thread:" << std::this_thread::get_id() << "current job:" << currentJob << ", jobCount" << jobCount << std::endl;
	IJob * job{ nullptr };
	JobType type = JobType::SHORT_TERM;
	//if (currentJob % 2)
	//	type = JobType::LONG_TERM;
	if(currentJob < jobCount)
	{
		job = JobManager::Instance().AllocateJob(type, nullptr, job_spawn, currentJob + 1, jobCount);
	}
	else
	{
		std::uint64_t nextLayerJobCount = jobCount == static_cast<std::uint64_t>(-1) ? jobCount : jobCount + 1;
		job = JobManager::Instance().AllocateJob(type, nullptr, job_spawn, 0, nextLayerJobCount);
	}
	JobManager::Instance().RunJob(job);
}

void task_generation_job()
{
	std::vector<JobSystem::IJob*> jobs;
	auto masterJob = JobManager::Instance().AllocateJob(JobType::SHORT_TERM, nullptr, []() {std::cout << "Master job created!" << std::endl; });
	jobs.push_back(masterJob);
	for (int i = 0;i < 100;i++)
	{
		auto job = JobManager::Instance().AllocateJob(JobType::SHORT_TERM, masterJob, job_spawn, 0, 2);
		jobs.push_back(job);
	}
	for (auto job : jobs)
		JobManager::Instance().RunJob(job);
	JobManager::Instance().WaitForCompletion(masterJob);
	std::cout << "All calculation job done!shutting up job system!" << std::endl;
	//JobManager::Instance().ShutDown();
	//std::cout << "JobSystem shut down!" << std::endl;
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