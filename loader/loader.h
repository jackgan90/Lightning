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
			DeserializeTask(const LoadTask& loaderTask, const Foundation::SharedFilePtr& file, 
				const std::shared_ptr<char>& buffer, bool ownBuffer);
			tbb::task* execute()override;
		private:
			LoadTask mLoadTask;
			Foundation::SharedFilePtr mFile;
			std::shared_ptr<char> mBuffer;
			bool mOwnBuffer;
		};

		class LIGHTNING_LOADER_API Loader : public Foundation::Singleton<Loader>
		{
		public:
			friend class Foundation::Singleton<Loader>;
			friend class DeserializeTask;
			void Finalize();
			void SetFileSystem(const Foundation::SharedFileSystemPtr& fs);
			void Load(const std::string& path, ISerializer* ser);
			~Loader();
		private:
			Loader();
			void DisposeFileAndBuffer(const std::string& path, const Foundation::SharedFilePtr& file);
			static void IOThread();
			bool mRunning;
			container::concurrent_queue<LoadTask> mTasks;
			container::concurrent_queue<std::string> mDisposedPathes;
			container::unordered_map<std::string, std::shared_ptr<char>> mBuffers;
			std::mutex mTaskQueueMutex;
			std::condition_variable mCondVar;
			Foundation::SharedFileSystemPtr mFileSystem;
		};
	}
}