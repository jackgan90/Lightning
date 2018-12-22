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
			IFile* FindFile(const char* fileName, FileAccess bitMask)override;
			bool SetRoot(const char* rootPath)override;
			const char* GetRoot() const override{ return mRoot.string().c_str(); }
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
			INTERFACECALL ~GeneralFile()override;
			FileSize INTERFACECALL GetSize()override;
			void INTERFACECALL SetFilePointer(FilePointerType type, FileAnchor anchor, FileSize offset)override;
			FileSize INTERFACECALL Read(char* buf, FileSize length)override;
			void INTERFACECALL Close()override;
			bool INTERFACECALL IsOpen()const override;
			const char* INTERFACECALL GetPath()const override;
			const char* INTERFACECALL GetName()const override;
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
