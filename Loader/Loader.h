#pragma once
#include <thread>
#include <mutex>
#include <functional>
#include <unordered_map>
#include "ILoader.h"
#include "Singleton.h"
#include "ISerializeBuffer.h"
#include "tbb/task.h"
#include "tbb/concurrent_queue.h"

namespace Lightning
{
	namespace Loading
	{
		using LoadFinishHandler = std::function<void(void*)>;
		struct LoadTask
		{
			ISerializer* serializer;
			std::string path;
		};

		class DeserializeTask : public tbb::task
		{
		public:
			DeserializeTask(const LoadTask& loaderTask, const std::shared_ptr<Foundation::IFile>& file, 
				ISerializeBuffer* buffer, bool ownFile);
			~DeserializeTask()override;
			tbb::task* execute()override;
		private:
			LoadTask mLoadTask;
			std::shared_ptr<Foundation::IFile> mFile;
			ISerializeBuffer* mBuffer;
			bool mOwnFile;
		};

		class Loader : public ILoader, public Foundation::Singleton<Loader>
		{
		public:
			void INTERFACECALL Finalize()override;
			void INTERFACECALL Load(const char* path, ISerializer* ser)override;
			INTERFACECALL ~Loader()override;
		private:
			friend class Foundation::Singleton<Loader>;
			friend class DeserializeTask;
			Loader();
			void DisposeFile(const std::string& path, Foundation::IFile* file);
			static void IOThread();
			bool mRunning;
			tbb::concurrent_queue<LoadTask> mTasks;
			tbb::concurrent_queue<std::string> mDisposedPathes;
			std::unordered_map<std::string, ISerializeBuffer*> mBuffers;
			std::mutex mTaskQueueMutex;
			std::condition_variable mCondVar;
			std::thread mLoaderIOThread;
		};
	}
}