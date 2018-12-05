#pragma once
#include <thread>
#include <mutex>
#include <functional>
#include "ILoader.h"
#include "Singleton.h"
#include "Container.h"
#include "ISerializeBuffer.h"
#include "tbb/task.h"

namespace Lightning
{
	namespace Loading
	{
		using Foundation::Container;
		using LoadFinishHandler = std::function<void(void*)>;
		struct LoadTask
		{
			ISerializer* serializer;
			std::string path;
		};

		class DeserializeTask : public tbb::task
		{
		public:
			DeserializeTask(const LoadTask& loaderTask, Foundation::IFile* file, 
				ISerializeBuffer* buffer, bool ownFile);
			~DeserializeTask()override;
			tbb::task* execute()override;
		private:
			LoadTask mLoadTask;
			Foundation::IFile* mFile;
			ISerializeBuffer* mBuffer;
			bool mOwnFile;
		};

		class Loader : public ILoader, public Foundation::Singleton<Loader>
		{
		public:
			void Finalize()override;
			void Load(const std::string& path, ISerializer* ser)override;
			~Loader()override;
		private:
			friend class Foundation::Singleton<Loader>;
			friend class DeserializeTask;
			Loader();
			void DisposeFile(const std::string& path, Foundation::IFile* file);
			static void IOThread();
			bool mRunning;
			Container::ConcurrentQueue<LoadTask> mTasks;
			Container::ConcurrentQueue<std::string> mDisposedPathes;
			Container::UnorderedMap<std::string, ISerializeBuffer*> mBuffers;
			std::mutex mTaskQueueMutex;
			std::condition_variable mCondVar;
			std::thread mLoaderIOThread;
		};
	}
}