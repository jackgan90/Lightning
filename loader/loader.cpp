#include "loader.h"
#include "texturedeserializer.h"
#include "shaderdeserializer.h"
#include "logger.h"
#include "tbb/task.h"

namespace Lightning
{
	namespace Loading
	{
		DeserializeTask::DeserializeTask(const LoadTask& loadTask, const Foundation::SharedFilePtr& file, char* buffer)
			:mLoadTask(loadTask), mFile(file), mBuffer(buffer)
		{

		}

		tbb::task* DeserializeTask::execute()
		{
			void* resource{ nullptr };
			mLoadTask.deserializer->Deserialize(mFile, mBuffer, &resource);
			mLoadTask.finishHandler(resource);
			delete[] mBuffer;
			mFile->Close();
			if (resource)
				delete resource;
			return nullptr;
		}

		Loader::Loader() : mRunning(true)
		{
			std::memset(mLoaders, 0, sizeof(mLoaders));
			mLoaders[LOAD_TYPE_TEXTURE] = new TextureDeserializer;
			mLoaders[LOAD_TYPE_SHADER] = new ShaderDeserializer;
			std::thread t(IOThread);
			t.detach();
		}

		Loader::~Loader()
		{
			for (std::size_t i = 0;i < LOAD_TYPE_NUM;++i)
			{
				delete mLoaders[i];
			}
		}

		void Loader::Finalize()
		{
			mRunning = false;
			mCondVar.notify_one();
		}

		void Loader::SetFileSystem(const Foundation::SharedFileSystemPtr& fs)
		{
			mFileSystem = fs;
		}

		void Loader::Load(LoadType type, const std::string& path, LoadFinishHandler handler)
		{
			LoadTask task;
			task.deserializer = mLoaders[type];
			task.finishHandler = handler;
			task.path = path;
			mTasks.push(task);
			mCondVar.notify_one();
		}

		//This thread only deals with IO related stuff.After finishing reading a file
		//it will hand over the buffer and file to a tbb task so that deserialization 
		//happens in tbb threads.
		void Loader::IOThread()
		{
			auto mgr = Loader::Instance();
			LOG_INFO("LoaderMgr IO Thread start!");
			while (mgr->mRunning)
			{
				LoadTask task;
				if (mgr->mTasks.try_pop(task))
				{
					LOG_INFO("Start to load file : {0}", task.path);
					auto file = mgr->mFileSystem->FindFile(task.path, Foundation::FileAccess::READ);
					static Foundation::SharedFilePtr EmptyFile;
					if (!file)
					{
						LOG_ERROR("Can't find file : {0}", task.path.c_str());
						task.finishHandler(nullptr);
						continue;
					}
					auto size = file->GetSize();
					if (size <= 0)
					{
						LOG_ERROR("File is empty : {0}", file->GetPath().c_str());
						file->Close();
						task.finishHandler(nullptr);
						continue;
					}
					file->SetFilePointer(Foundation::FilePointerType::Read, Foundation::FileAnchor::Begin, 0);
					char* buffer = new char[std::size_t(size)];
					auto readSize = file->Read(buffer, size);
					if (readSize < size)
					{
						LOG_ERROR("Unable to read whole file : {0}", file->GetPath());
						file->Close();
						delete[] buffer;
						task.finishHandler(nullptr);
						continue;
					}
					LOG_INFO("Loader finished reading file : {0}, buffer size : {1}", task.path, size);
					auto deserializeTask = new (tbb::task::allocate_root()) DeserializeTask(task, file, buffer);
					tbb::task::enqueue(*deserializeTask);
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