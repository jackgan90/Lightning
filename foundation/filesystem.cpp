#include <algorithm>
#include "logger.h"
#include "filesystem.h"
#include "common.h"
#include "configmanager.h"

namespace Lightning
{
	namespace Foundation
	{
		extern Logger logger;
		GeneralFile::GeneralFile():mSizeDirty(true), mFile(nullptr), mAccess(FileAccess::READ)
		{

		}

		GeneralFile::GeneralFile(const std::string& path, FileAccess bitmask):mPath(path), mSizeDirty(true), mFile(nullptr), mAccess(bitmask)
		{

		}

		GeneralFile::GeneralFile(GeneralFile&& f)
		{
			MoveFrom(std::move(f));
		}

		GeneralFile& GeneralFile::operator=(GeneralFile&& f)
		{
			if (this != &f)
			{
				MoveFrom(std::move(f));
			}
			return *this;
		}

		GeneralFile::~GeneralFile()
		{
			Close();
		}

		void GeneralFile::MoveFrom(GeneralFile&& f)
		{
			mSize = f.mSize;
			mPath = f.mPath;
			mSizeDirty = f.mSizeDirty;
			mFile = std::move(f.mFile);
			mAccess = f.mAccess;
			f.mFile = nullptr;
		}
		

		void GeneralFile::Close()
		{
			if (mFile)
			{
				mFile->close();
			}
		}

		FileSize GeneralFile::GetSize()
		{
			if (mSizeDirty)
			{
				CalculateFileSize();
				mSizeDirty = false;
			}
			return mSize;
		}

		void GeneralFile::SetFilePointer(FilePointerType type, FileAnchor anchor, FileSize offset)
		{
			OpenFile();
			switch (type)
			{
			case FilePointerType::Read:
				switch (anchor)
				{
				case FileAnchor::Begin:
					mFile->seekg(offset, std::ios_base::beg);
					break;
				case FileAnchor::Current:
					mFile->seekg(offset, std::ios_base::cur);
					break;
				case FileAnchor::End:
					mFile->seekg(offset, std::ios_base::end);
					break;
				}
				break;
			case FilePointerType::Write:
				switch (anchor)
				{
				case FileAnchor::Begin:
					mFile->seekp(offset, std::ios_base::beg);
					break;
				case FileAnchor::Current:
					mFile->seekp(offset, std::ios_base::cur);
					break;
				case FileAnchor::End:
					mFile->seekp(offset, std::ios_base::end);
					break;
				}
				break;
			}
		}


		void GeneralFile::CalculateFileSize()
		{
			OpenFile();
			auto fp = mFile->tellg();
			mFile->seekg(0, std::ios_base::end);
			mSize = mFile->tellg();
			mFile->seekg(fp, std::ios_base::beg);
		}

		void GeneralFile::OpenFile()
		{
			if (!mFile)
			{
				int mode = 0;
				if ((mAccess & FileAccess::READ) == FileAccess::READ)
					mode |= std::fstream::in;
				if ((mAccess & FileAccess::WRITE) == FileAccess::WRITE)
					mode |= std::fstream::out;
				mFile = std::make_unique<boost::filesystem::fstream>(mPath.string(), std::fstream::binary | mode);
			}
		}


		FileSize GeneralFile::Read(char* buf, FileSize length)
		{
			FileSize size = GetSize();
			FileSize readSize = std::min(size, length);
			mFile->read(buf, readSize);
			return *mFile ? readSize : mFile->gcount();
		}

		const std::string GeneralFile::GetPath()const
		{
			return mPath.string();
		}

		const std::string GeneralFile::GetName()const
		{
			return mPath.filename().string();
		}


		GeneralFileSystem::GeneralFileSystem()
		{
			//path p = boost::filesystem::current_path();
			mRoot = ConfigManager::Instance()->GetConfig().ResourceRoot;
		}
		
		GeneralFileSystem::~GeneralFileSystem()
		{

		}

		SharedFilePtr GeneralFileSystem::FindFile(const std::string& filename, FileAccess bitmask)
		{
			//TODO : multithreaded access must be resolved
			auto cachedFile = mCachedFiles.find(filename);
			if (cachedFile != mCachedFiles.end())
				return cachedFile->second;
			const boost::filesystem::recursive_directory_iterator end;
			try
			{
				const auto it = std::find_if(boost::filesystem::recursive_directory_iterator(mRoot), end, 
					[&filename](const boost::filesystem::directory_entry& e) {
					return e.path().filename() == filename;
				});
				if (it != end)
				{
					mCachedFiles.insert(std::make_pair(filename, SharedFilePtr(new GeneralFile(it->path().string(), bitmask))));
					return mCachedFiles[filename];
				}
			}
			catch (const boost::filesystem::filesystem_error& e)
			{
				logger.Log(LogLevel::Error, e.what());
			}
			return SharedFilePtr();
		}

		bool GeneralFileSystem::SetRoot(std::string root_path)
		{
			mRoot = root_path;
			return true;
		}

	}
}