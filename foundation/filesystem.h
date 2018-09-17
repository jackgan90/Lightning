#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <boost/filesystem.hpp>
#include "foundationexportdef.h"
#include "singleton.h"
#include "enumoperation.h"

namespace Lightning
{
	namespace Foundation
	{
		using FileSize = std::streamsize;

		enum class FilePointerType
		{
			Read,
			Write
		};

		enum class FileAnchor
		{
			Begin,
			Current,
			End
		};

		enum class FileAccess : std::uint8_t
		{
			READ = 0x01,
			WRITE = 0x02,
		};
		ENABLE_ENUM_BITMASK_OPERATORS(FileAccess)
		class LIGHTNING_FOUNDATION_API IFile
		{
		public:
			virtual ~IFile() = default;
			virtual FileSize GetSize() = 0;
			virtual FileSize Read(char* buf, FileSize length) = 0;
			virtual void SetFilePointer(FilePointerType type, FileAnchor anchor, FileSize offset) = 0;
			virtual void Close() = 0;
			virtual const std::string GetPath()const = 0;
			virtual const std::string GetName()const = 0;
		};
		using SharedFilePtr = std::shared_ptr<IFile>;

		class LIGHTNING_FOUNDATION_API IFileSystem
		{
		public:
			virtual ~IFileSystem() = default;
			virtual SharedFilePtr FindFile(const std::string& filename, FileAccess bitmask) = 0;
			virtual bool SetRoot(std::string root_path) = 0;
			virtual const std::string GetRoot() const = 0;
		};
		using SharedFileSystemPtr = std::shared_ptr<IFileSystem>;

		class LIGHTNING_FOUNDATION_API GeneralFileSystem : public IFileSystem
		{
		public:
			GeneralFileSystem();
			~GeneralFileSystem()override;
			SharedFilePtr FindFile(const std::string& fileName, FileAccess bitMask)override;
			bool SetRoot(std::string rootPath)override;
			const std::string GetRoot() const override{ return mRoot.string(); }
		protected:
			boost::filesystem::path mRoot;
			std::unordered_map<std::string, SharedFilePtr> mCachedFiles;
		};

		class GeneralFile : public IFile
		{
		public:
			friend class GeneralFileSystem;
			GeneralFile();
			GeneralFile(const std::string& path, FileAccess bitmask);
			//should not copy an existing file which may cause chaos file access.
			GeneralFile(const GeneralFile& f) = delete;
			GeneralFile& operator=(const GeneralFile& f) = delete;
			GeneralFile(GeneralFile&& f);
			GeneralFile& operator=(GeneralFile&& f);
			~GeneralFile()override;
			FileSize GetSize()override;
			void SetFilePointer(FilePointerType type, FileAnchor anchor, FileSize offset)override;
			FileSize Read(char* buf, FileSize length)override;
			void Close()override;
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
		};

	}
}
