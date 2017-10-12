#include <algorithm>
#include "logger.h"
#include "filesystem.h"
#include "common.h"
#include "configmanager.h"

namespace LightningGE
{
	namespace Foundation
	{
		extern Logger logger;
		GeneralFile::GeneralFile():m_sizeDirty(true), m_file(nullptr), m_access(FileAccess::ACCESS_READ)
		{

		}

		GeneralFile::GeneralFile(const std::string& path, FileAccess bitmask):m_path(path), m_sizeDirty(true), m_file(nullptr), m_access(bitmask)
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
			m_size = f.m_size;
			m_path = f.m_path;
			m_sizeDirty = f.m_sizeDirty;
			m_file = std::move(f.m_file);
			m_access = f.m_access;
			f.m_file = nullptr;
		}
		

		void GeneralFile::Close()
		{
			if (m_file)
			{
				m_file->close();
			}
		}

		FileSize GeneralFile::GetSize()
		{
			if (m_sizeDirty)
			{
				CalculateFileSize();
				m_sizeDirty = false;
			}
			return m_size;
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
					m_file->seekg(offset, std::ios_base::beg);
					break;
				case FileAnchor::Current:
					m_file->seekg(offset, std::ios_base::cur);
					break;
				case FileAnchor::End:
					m_file->seekg(offset, std::ios_base::end);
					break;
				}
				break;
			case FilePointerType::Write:
				switch (anchor)
				{
				case FileAnchor::Begin:
					m_file->seekp(offset, std::ios_base::beg);
					break;
				case FileAnchor::Current:
					m_file->seekp(offset, std::ios_base::cur);
					break;
				case FileAnchor::End:
					m_file->seekp(offset, std::ios_base::end);
					break;
				}
				break;
			}
		}


		void GeneralFile::CalculateFileSize()
		{
			OpenFile();
			auto fp = m_file->tellg();
			m_file->seekg(0, std::ios_base::end);
			m_size = m_file->tellg();
			m_file->seekg(fp, std::ios_base::beg);
		}

		void GeneralFile::OpenFile()
		{
			if (!m_file)
			{
				int mode = 0;
				if (m_access & FileAccess::ACCESS_READ)
					mode |= std::fstream::in;
				if (m_access & FileAccess::ACCESS_WRITE)
					mode |= std::fstream::out;
				m_file = std::make_unique<boost::filesystem::fstream>(m_path.string(), std::fstream::binary | mode);
			}
		}


		FileSize GeneralFile::Read(char* buf, FileSize length)
		{
			FileSize size = GetSize();
			FileSize readSize = std::min(size, length);
			m_file->read(buf, readSize);
			return *m_file ? readSize : m_file->gcount();
		}

		const std::string GeneralFile::GetPath()const
		{
			return m_path.string();
		}

		const std::string GeneralFile::GetName()const
		{
			return m_path.filename().string();
		}


		GeneralFileSystem::GeneralFileSystem()
		{
			//path p = boost::filesystem::current_path();
			m_root = ConfigManager::Instance()->GetConfig().ResourceRoot;
		}
		
		GeneralFileSystem::~GeneralFileSystem()
		{

		}

		FilePtr GeneralFileSystem::FindFile(const std::string& filename, FileAccess bitmask)
		{
			//TODO : multithreaded access must be resolved
			auto cachedFile = m_cachedFiles.find(filename);
			if (cachedFile != m_cachedFiles.end())
				return cachedFile->second;
			const boost::filesystem::recursive_directory_iterator end;
			try
			{
				const auto it = std::find_if(boost::filesystem::recursive_directory_iterator(m_root), end, 
					[&filename](const boost::filesystem::directory_entry& e) {
					return e.path().filename() == filename;
				});
				if (it != end)
				{
					m_cachedFiles.insert(std::make_pair(filename, FilePtr(new GeneralFile(it->path().string(), bitmask))));
					return m_cachedFiles[filename];
				}
			}
			catch (const boost::filesystem::filesystem_error& e)
			{
				logger.Log(LogLevel::Error, e.what());
			}
			return FilePtr();
		}

		bool GeneralFileSystem::SetRoot(std::string root_path)
		{
			m_root = root_path;
			return true;
		}

	}
}