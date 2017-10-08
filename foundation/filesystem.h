#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <boost/filesystem.hpp>
#include "foundationexportdef.h"

namespace LightningGE
{
	namespace Foundation
	{
		typedef long FileSize;
		enum FilePointerType
		{
			Read,
			Write
		};

		enum FileAnchor
		{
			Begin,
			Current,
			End
		};

		enum FileAccess
		{
			ACCESS_READ = 1,
			ACCESS_WRITE = 2,
		};
		class LIGHTNINGGE_FOUNDATION_API IFile
		{
		public:
			virtual ~IFile() {}
			virtual FileSize GetSize() = 0;
			virtual FileSize Read(char* buf, FileSize length) = 0;
			virtual void SetFilePointer(FilePointerType type, FileAnchor anchor, FileSize offset) = 0;
			virtual void Close() = 0;
			virtual const std::string GetPath()const = 0;
			virtual const std::string GetName()const = 0;
		};
		typedef std::shared_ptr<IFile> FilePtr;

		class LIGHTNINGGE_FOUNDATION_API IFileSystem
		{
		public:
			virtual ~IFileSystem() {}
			virtual FilePtr FindFile(const std::string& filename, FileAccess bitmask) = 0;
			virtual bool SetRoot(std::string root_path) = 0;
			virtual const std::string GetRoot() const = 0;
		};
		typedef std::shared_ptr<IFileSystem> FileSystemPtr;

		class LIGHTNINGGE_FOUNDATION_API GeneralFileSystem : public IFileSystem
		{
		public:
			GeneralFileSystem();
			~GeneralFileSystem()override;
			FilePtr FindFile(const std::string& filename, FileAccess bitmask)override;
			bool SetRoot(std::string root_path)override;
			const std::string GetRoot() const { return m_root.string(); }
		protected:
			boost::filesystem::path m_root;
			std::unordered_map<std::string, FilePtr> m_cachedFiles;
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
			FileSize m_size;
			boost::filesystem::path m_path;
			bool m_sizeDirty;
			std::fstream* m_file;
			FileAccess m_access;
		};

	}
}