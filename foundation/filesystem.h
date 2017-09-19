#pragma once
#include <string>
#include "foundationexportdef.h"

namespace LightningGE
{
	namespace Foundation
	{
		using FileSize = long;
		class LIGHTNINGGE_FOUNDATION_API IFile
		{
		public:
			virtual ~IFile() {}
			virtual FileSize GetSize() const = 0;
			virtual FileSize Read(char* buf) = 0;
		};

		class LIGHTNINGGE_FOUNDATION_API IFileSystem
		{
		public:
			virtual ~IFileSystem() {}
			virtual IFile* FindFile(std::string path) = 0;
			virtual bool SetRoot(std::string root_path) = 0;
		};

		class GeneralFileSystem : public IFileSystem
		{
		public:
			GeneralFileSystem();
			~GeneralFileSystem()override;
			IFile* FindFile(std::string path)override;
			bool SetRoot(std::string root_path)override;
		protected:
			std::string m_root;
		};

		class GeneralFile : public IFile
		{
		public:
			friend class GeneralFileSystem;
			GeneralFile();
			~GeneralFile()override;
			FileSize GetSize()const override;
			FileSize Read(char* buf)override;
		protected:
			FileSize m_size;
		};

	}
}