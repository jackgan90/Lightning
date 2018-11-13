#pragma once
#include "JobManager.h"

#define JOB_RUN_ON_THREAD(jobName, jobType, parentJob, threadId, function, ...)\
	auto handle##jobName = JobSystem::JobManager::Instance().AllocateJob(jobType, parentJob, (function), __VA_ARGS__);\
	JobSystem::JobManager::Instance().RunJob(handle##jobName, threadId)

#define JOB_RUN(jobName, jobType, parentJob, function, ...)\
	auto handle##jobName = JobSystem::JobManager::Instance().AllocateJob(jobType, parentJob, (function), __VA_ARGS__);\
	JobSystem::JobManager::Instance().RunJob(handle##jobName)


#define QUICK_JOB_RUN(jobName, function, ...)\
JOB_RUN(jobName, JobSystem::JOB_TYPE_FOREGROUND, JobSystem::INVALID_JOB_HANDLE, function, __VA_ARGS__)

#define SLOW_JOB_RUN(jobName, function, ...)\
JOB_RUN(jobName, JobSystem::JOB_TYPE_BACKGROUND, JobSystem::INVALID_JOB_HANDLE, function, __VA_ARGS__)


#define JOB_WAIT(jobHandle) JobSystem::JobManager::Instance().WaitForCompletion(jobHandle)

#define NEW_JOB(jobType, parentJob, function, ...)\
	JobSystem::JobManager::Instance().AllocateJob(jobType, parentJob, (function), __VA_ARGS__)

#define NEW_QUICK_JOB(parentJob, function, ...) NEW_JOB(JobSystem::JOB_TYPE_FOREGROUND, parentJob, function, __VA_ARGS__)
#define NEW_SLOW_JOB(parentJob, function, ...) NEW_JOB(JobSystem::JOB_TYPE_BACKGROUND, parentJob, function, __VA_ARGS__)

#define JOB_START(jobHandle) JobSystem::JobManager::Instance().RunJob(jobHandle);