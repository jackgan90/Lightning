#pragma once
#include <fstream>
#include "EnumOperation.h"
#include "RefObject.h"

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

		class IFile : public Plugins::IRefObject
		{
		public:
			virtual FileSize GetSize() = 0;
			virtual FileSize Read(char* buf, FileSize length) = 0;
			virtual void SetFilePointer(FilePointerType type, FileAnchor anchor, FileSize offset) = 0;
			virtual void Close() = 0;
			virtual bool IsOpen()const = 0;
			virtual const std::string GetPath()const = 0;
			virtual const std::string GetName()const = 0;
		};

		class IFileSystem
		{
		public:
			virtual ~IFileSystem() = default;
			//Thread unsafe.Application must call this method from loader IO thread
			virtual IFile* FindFile(const std::string& filename, FileAccess bitmask) = 0;
			virtual bool SetRoot(std::string root_path) = 0;
			virtual const std::string GetRoot() const = 0;
		};
	}
}
