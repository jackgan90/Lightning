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

		struct IFile : Plugins::IRefObject
		{
			virtual FileSize INTERFACECALL GetSize() = 0;
			virtual FileSize INTERFACECALL Read(char* buf, FileSize length) = 0;
			virtual void INTERFACECALL SetFilePointer(FilePointerType type, FileAnchor anchor, FileSize offset) = 0;
			virtual void INTERFACECALL Close() = 0;
			virtual bool INTERFACECALL IsOpen()const = 0;
			virtual const char* INTERFACECALL GetPath()const = 0;
			virtual const char* INTERFACECALL GetName()const = 0;
		};

		struct IFileSystem
		{
			virtual INTERFACECALL ~IFileSystem() = default;
			//Thread unsafe.Application must call this method from loader IO thread
			virtual IFile* INTERFACECALL FindFile(const char* filename, FileAccess bitmask) = 0;
			virtual bool INTERFACECALL SetRoot(const char* root_path) = 0;
			virtual const char* INTERFACECALL GetRoot() const = 0;
		};
	}
}
