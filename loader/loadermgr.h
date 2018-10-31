#pragma once
#include <thread>
#include <mutex>
#include "loaderexportdef.h"
#include "singleton.h"
#include "iloader.h"
#include "container.h"
#include "filesystem.h"

namespace Lightning
{
	namespace Loader
	{
		using Foundation::container;
		struct LoaderTask
		{
			ILoader* owner;
			LoadFinishHandler finishHandler;
			std::string path;
		};

		class LIGHTNING_LOADER_API LoaderMgr : public Foundation::Singleton<LoaderMgr>
		{
		public:
			friend class Foundation::Singleton<LoaderMgr>;
			friend class BaseLoader;
			void Finalize();
			void SetFileSystem(const Foundation::SharedFileSystemPtr& fs);
			ILoader* GetLoader(LoaderType type);
			~LoaderMgr();
		private:
			void AddLoaderTask(const LoaderTask& task);
			LoaderMgr();
			static void IOThread();
			bool mRunning;
			container::concurrent_queue<LoaderTask> mTasks;
			std::mutex mTaskQueueMutex;
			std::condition_variable mCondVar;
			Foundation::SharedFileSystemPtr mFileSystem;
			ILoader* mLoaders[LOADER_TYPE_NUM];
		};
	}
}