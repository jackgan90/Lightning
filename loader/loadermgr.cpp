#include "loadermgr.h"
#include "textureloader.h"
#include "shaderloader.h"
#include "logger.h"
#include "tbb/task.h"

namespace Lightning
{
	namespace Loader
	{

		LoaderMgr::LoaderMgr() : mRunning(true)
		{
			std::memset(mLoaders, 0, sizeof(mLoaders));
			mLoaders[LOADER_TYPE_TEXTURE] = new TextureLoader;
			mLoaders[LOADER_TYPE_SHADER] = new ShaderLoader;
			std::thread t(IOThread);
			t.detach();
		}

		LoaderMgr::~LoaderMgr()
		{
			for (std::size_t i = 0;i < LOADER_TYPE_NUM;++i)
			{
				delete mLoaders[i];
			}
		}

		void LoaderMgr::Finalize()
		{
			mRunning = false;
			mCondVar.notify_one();
		}

		void LoaderMgr::SetFileSystem(const Foundation::SharedFileSystemPtr& fs)
		{
			mFileSystem = fs;
		}

		ILoader* LoaderMgr::GetLoader(LoaderType type)
		{
			return mLoaders[type];
		}

		void LoaderMgr::AddLoaderTask(const LoaderTask& task)
		{
			mTasks.push(task);
			mCondVar.notify_one();
		}

		//This thread only deals with IO related stuff.After finishing reading a file
		//it will hand over the buffer and file to a tbb task so that deserialization 
		//happens in tbb threads.
		void LoaderMgr::IOThread()
		{
			auto mgr = LoaderMgr::Instance();
			LOG_INFO("LoaderMgr IO Thread start!");
			while (mgr->mRunning)
			{
				LoaderTask task;
				if (mgr->mTasks.try_pop(task))
				{
					LOG_INFO("Start to load file : {0}", task.path);
					auto file = mgr->mFileSystem->FindFile(task.path, Foundation::FileAccess::READ);
					static Foundation::SharedFilePtr EmptyFile;
					if (!file)
					{
						LOG_ERROR("Can't find file : {0}", task.path.c_str());
						static_cast<BaseLoader*>(task.owner)->OnFileBufferReady(EmptyFile, nullptr, task);
						continue;
					}
					auto size = file->GetSize();
					if (size <= 0)
					{
						LOG_WARNING("File is empty : {0}", file->GetPath().c_str());
						file->Close();
						static_cast<BaseLoader*>(task.owner)->OnFileBufferReady(EmptyFile, nullptr, task);
						continue;
					}
					file->SetFilePointer(Foundation::FilePointerType::Read, Foundation::FileAnchor::Begin, 0);
					char* buffer = new char[std::size_t(size)];
					auto readSize = file->Read(buffer, size);
					LOG_INFO("Loader finished reading file : {0}, buffer size : {1}", task.path, size);
					auto constructTask = new (tbb::task::allocate_root()) ConstructObjectTask(task, file, buffer);
					tbb::task::enqueue(*constructTask);
				}
				else
				{
					std::unique_lock<std::mutex> lock(mgr->mTaskQueueMutex);
					mgr->mCondVar.wait(lock);
				}
			}
			LOG_INFO("Loader IO Thread exit!");
		}

	}
}