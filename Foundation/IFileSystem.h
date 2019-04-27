#pragma once
#include <fstream>
#include "EnumOperation.h"

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

		struct IFile
		{
			virtual ~IFile() = default;
			virtual FileSize GetSize() = 0;
			virtual FileSize Read(char* buf, FileSize length) = 0;
			virtual void SetFilePointer(FilePointerType type, FileAnchor anchor, FileSize offset) = 0;
			virtual void Close() = 0;
			virtual bool IsOpen()const = 0;
			virtual std::string GetPath()const = 0;
			virtual std::string GetName()const = 0;
		};

		struct IFileSystem
		{
			virtual ~IFileSystem() = default;
			//Thread unsafe.Application must call this method from loader IO thread
			virtual std::shared_ptr<IFile> FindFile(const std::string& filename, FileAccess bitmask) = 0;
			virtual bool SetRoot(const std::string& root_path) = 0;
			virtual std::string GetRoot() const = 0;
		};
	}
}
