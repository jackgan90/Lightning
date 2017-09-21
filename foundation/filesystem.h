#pragma once
#include <string>
#include <unordered_map>
#include <memory>
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
			virtual const std::string GetPath()const = 0;
			virtual void SetPath(const std::string& path) = 0;
		};
		using FilePtr = std::shared_ptr<IFile>;

		class LIGHTNINGGE_FOUNDATION_API IFileSystem
		{
		public:
			virtual ~IFileSystem() {}
			virtual FilePtr FindFile(const std::string& filename) = 0;
			virtual bool SetRoot(std::string root_path) = 0;
			virtual const std::string GetRoot() const = 0;
		};

		class LIGHTNINGGE_FOUNDATION_API GeneralFileSystem : public IFileSystem
		{
		public:
			GeneralFileSystem();
			~GeneralFileSystem()override;
			FilePtr FindFile(const std::string& filename)override;
			bool SetRoot(std::string root_path)override;
			const std::string GetRoot() const { return m_root; }
		protected:
			std::string m_root;
			std::unordered_map<std::string, FilePtr> m_cachedFiles;
		};

		class GeneralFile : public IFile
		{
		public:
			friend class GeneralFileSystem;
			GeneralFile();
			GeneralFile(const std::string& path);
			//TODO : consider necessity of copy/move constructor and assignment operator
			~GeneralFile()override;
			FileSize GetSize()const override;
			FileSize Read(char* buf)override;
			const std::string GetPath()const override;
			void SetPath(const std::string& path)override;
		protected:
			FileSize m_size;
			std::string m_path;
		};

	}
}