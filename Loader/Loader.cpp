#include "Loader.h"
#include "Logger.h"
#include "tbb/task.h"
#include "Environment.h"

namespace Lightning
{
	namespace Loading
	{
		DeserializeTask::DeserializeTask(const LoadTask& loadTask, Foundation::IFile* file, 
			const std::shared_ptr<char>& buffer, bool ownBuffer)
			:mLoadTask(loadTask), mFile(file), mBuffer(buffer), mOwnBuffer(ownBuffer)
		{
			mFile->AddRef();
		}

		DeserializeTask::~DeserializeTask()
		{
			mFile->Release();
		}


		tbb::task* DeserializeTask::execute()
		{
			mLoadTask.serializer->Deserialize(mFile, mBuffer.get());
			mLoadTask.serializer->Dispose();
			if (mOwnBuffer)
			{
				Loader::Instance()->DisposeFileAndBuffer(mLoadTask.path, mFile);
			}
			return nullptr;
		}

		Loader::Loader() : mRunning(true), mFileSystem(nullptr)
		{
			std::thread t(IOThread);
			t.detach();
		}

		Loader::~Loader()
		{
			if (mFileSystem)
				mFileSystem->Release();
		}

		void Loader::Finalize()
		{
			mRunning = false;
			mCondVar.notify_one();
		}

		void Loader::SetFileSystem(Foundation::IFileSystem* fs)
		{
			if (mFileSystem)
				mFileSystem->Release();
			mFileSystem = fs;
			if (mFileSystem)
				mFileSystem->AddRef();
		}

		void Loader::Load(const std::string& path, ISerializer* ser)
		{
			LoadTask task;
			task.serializer = ser;
			task.path = path;
			mTasks.push(task);
			mCondVar.notify_one();
		}

		void Loader::DisposeFileAndBuffer(const std::string& path, Foundation::IFile* file)
		{
			mDisposedPathes.push(path);
			file->Close();
			mCondVar.notify_one();
		}

		//This thread only deals with IO related stuff.After finishing reading a file
		//it will hand over the buffer and file to a tbb task so that deserialization 
		//happens in tbb threads.
		void Loader::IOThread()
		{
			Foundation::Environment::LoaderIOThreadID = std::this_thread::get_id();
			auto mgr = Loader::Instance();
			LOG_INFO("LoaderMgr IO Thread start!");
			while (mgr->mRunning)
			{
				std::string path;
				while (mgr->mDisposedPathes.try_pop(path))
				{
					mgr->mBuffers.erase(path);
				}
				LoadTask task;
				if (mgr->mTasks.try_pop(task))
				{
					LOG_INFO("Start to load file : {0}", task.path);
					auto file = mgr->mFileSystem->FindFile(task.path, Foundation::FileAccess::READ);
					if (!file)
					{
						LOG_ERROR("Can't find file : {0}", task.path.c_str());
						continue;
					}
					if (file->IsOpen())
					{
						auto deserializeTask = new (tbb::task::allocate_root()) DeserializeTask(task, file, mgr->mBuffers[task.path], false);
						tbb::task::enqueue(*deserializeTask);
						continue;
					}
					auto size = file->GetSize();
					if (size <= 0)
					{
						LOG_ERROR("File is empty : {0}", file->GetPath().c_str());
						file->Close();
						continue;
					}
					file->SetFilePointer(Foundation::FilePointerType::Read, Foundation::FileAnchor::Begin, 0);
					auto sharedBuffer = std::shared_ptr<char>(new char[std::size_t(size + 1)], std::default_delete<char[]>());
					char* buffer = sharedBuffer.get();
					buffer[size] = 0;
					auto readSize = file->Read(buffer, size);
					if (readSize < size)
					{
						LOG_ERROR("Unable to read whole file : {0}", file->GetPath());
						file->Close();
						sharedBuffer.reset();
						continue;
					}
					mgr->mBuffers[task.path] = sharedBuffer;
					LOG_INFO("Loader finished reading file : {0}, buffer size : {1}", task.path, size);
					auto deserializeTask = new (tbb::task::allocate_root()) DeserializeTask(task, file, sharedBuffer, true);
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