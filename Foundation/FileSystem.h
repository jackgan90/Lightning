#pragma once
#include <string>
#include <memory>
#include <boost/filesystem.hpp>
#include "Container.h"
#include "Singleton.h"
#include "IFileSystem.h"

namespace Lightning
{
	namespace Foundation
	{

		class GeneralFileSystem : public IFileSystem
		{
		public:
			GeneralFileSystem();
			~GeneralFileSystem()override;
			IFile* FindFile(const std::string& fileName, FileAccess bitMask)override;
			bool SetRoot(std::string rootPath)override;
			const std::string GetRoot() const override{ return mRoot.string(); }
		protected:
			boost::filesystem::path mRoot;
			Container::UnorderedMap<std::string, IFile*> mCachedFiles;
		};

		class GeneralFile : public IFile
		{
		public:
			friend class GeneralFileSystem;
			GeneralFile();
			GeneralFile(const std::string& path, FileAccess bitmask);
			//should not copy an existing file which may cause chaos file access.
			GeneralFile(GeneralFile&& f);
			GeneralFile& operator=(GeneralFile&& f);
			~GeneralFile()override;
			FileSize GetSize()override;
			void SetFilePointer(FilePointerType type, FileAnchor anchor, FileSize offset)override;
			FileSize Read(char* buf, FileSize length)override;
			void Close()override;
			bool IsOpen()const override;
			const std::string GetPath()const override;
			const std::string GetName()const override;
		protected:
			void CalculateFileSize();
			void OpenFile();
			void MoveFrom(GeneralFile&& f);
			FileSize mSize;
			boost::filesystem::path mPath;
			bool mSizeDirty;
			std::unique_ptr<std::fstream> mFile;
			FileAccess mAccess;
			REF_OBJECT_OVERRIDE(GeneralFile)
		};

	}
}
