#include "baseloader.h"
#include "loadermgr.h"

namespace Lightning
{
	namespace Loader
	{
		ConstructObjectTask::ConstructObjectTask(const LoaderTask& loaderTask, const Foundation::SharedFilePtr& file, char* buffer)
			:mLoaderTask(loaderTask), mFile(file), mBuffer(buffer)
		{

		}

		tbb::task* ConstructObjectTask::execute()
		{
			auto pLoader = static_cast<BaseLoader*>(mLoaderTask.owner);
			pLoader->OnFileBufferReady(mFile, mBuffer, mLoaderTask);
			delete[] mBuffer;
			mFile->Close();
			return nullptr;
		}

		BaseLoader::BaseLoader()
		{

		}

		void BaseLoader::Load(const std::string& path, LoadFinishHandler callback)
		{
			LoaderTask task;
			task.owner = this;
			task.finishHandler = callback;
			task.path = path;
			LoaderMgr::Instance()->AddLoaderTask(task);
		}

		void BaseLoader::OnFileBufferReady(const Foundation::SharedFilePtr& file, char* buffer, const LoaderTask& task)
		{
			auto pObject = ConstructObject(file, buffer);
			task.finishHandler(pObject);
		}
	}
}