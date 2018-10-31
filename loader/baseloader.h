#pragma once
#include "iloader.h"
#include "container.h"
#include "filesystem.h"
#include "tbb/task.h"
#include "loadermgr.h"

namespace Lightning
{
	namespace Loader
	{
		class ConstructObjectTask : public tbb::task
		{
		public:
			ConstructObjectTask(const LoaderTask& loaderTask, const Foundation::SharedFilePtr& file, char* buffer);
			tbb::task* execute()override;
		private:
			LoaderTask mLoaderTask;
			Foundation::SharedFilePtr mFile;
			char* mBuffer;
		};

		class LIGHTNING_LOADER_API BaseLoader : public ILoader
		{
		public:
			friend class ConstructObjectTask;
			friend class LoaderMgr;
			BaseLoader();
			void Load(const std::string& path, LoadFinishHandler callback)override;
		protected:
			//Only called by tbb task
			void OnFileBufferReady(const Foundation::SharedFilePtr& file, char* buffer, const LoaderTask& task);
			virtual void* ConstructObject(const Foundation::SharedFilePtr& file, char* buffer) = 0;
		};
	}
}