#pragma once
#include <thread>
#include <mutex>
#include <functional>
#include "loaderexportdef.h"
#include "singleton.h"
#include "iserializer.h"
#include "container.h"
#include "tbb/task.h"
#include "filesystem.h"

namespace Lightning
{
	namespace Loading
	{
		using Foundation::container;
		using LoadFinishHandler = std::function<void(void*)>;
		struct LoadTask
		{
			ISerializer* serializer;
			std::string path;
		};

		class DeserializeTask : public tbb::task
		{
		public:
			DeserializeTask(const LoadTask& loaderTask, const Foundation::SharedFilePtr& file, char* buffer);
			tbb::task* execute()override;
		private:
			LoadTask mLoadTask;
			Foundation::SharedFilePtr mFile;
			char* mBuffer;
		};

		class LIGHTNING_LOADER_API Loader : public Foundation::Singleton<Loader>
		{
		public:
			friend class Foundation::Singleton<Loader>;
			friend class BaseLoader;
			void Finalize();
			void SetFileSystem(const Foundation::SharedFileSystemPtr& fs);
			void RegisterSerializer(LoadType type, ISerializer* ser);
			void Load(LoadType type, const std::string& path);
			~Loader();
		private:
			Loader();
			static void IOThread();
			bool mRunning;
			container::concurrent_queue<LoadTask> mTasks;
			std::mutex mTaskQueueMutex;
			std::condition_variable mCondVar;
			Foundation::SharedFileSystemPtr mFileSystem;
			ISerializer* mLoaders[LOAD_TYPE_NUM];
		};
	}
}