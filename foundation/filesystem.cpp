#include "filesystem.h"
#include <boost/filesystem.hpp>
#include <algorithm>

using namespace boost::filesystem;
using std::string;
namespace LightningGE
{
	namespace Foundation
	{
		GeneralFile::GeneralFile()
		{

		}

		GeneralFile::GeneralFile(const std::string& path):m_path(path)
		{

		}

		GeneralFile::~GeneralFile()
		{

		}

		FileSize GeneralFile::GetSize() const
		{
			return m_size;
		}

		FileSize GeneralFile::Read(char* buf)
		{
			return 0;
		}

		const std::string GeneralFile::GetPath()const
		{
			return m_path;
		}

		void GeneralFile::SetPath(const std::string& path)
		{
			m_path = path;
			//TODO : should do something involving file already opened
		}

		GeneralFileSystem::GeneralFileSystem()
		{
			path p = boost::filesystem::current_path();
			m_root = p.string();
		}
		
		GeneralFileSystem::~GeneralFileSystem()
		{

		}

		FilePtr GeneralFileSystem::FindFile(const std::string& filename)
		{
			//TODO : multithreaded access must be resolved
			auto cachedFile = m_cachedFiles.find(filename);
			if (cachedFile != m_cachedFiles.end())
				return cachedFile->second;
			const recursive_directory_iterator end;
			const auto it = std::find_if(recursive_directory_iterator(path(m_root)), end, 
				[&filename](const directory_entry& e) {
				return e.path().filename() == filename;
			});
			if (it != end)
			{
				m_cachedFiles.insert(std::make_pair(filename, FilePtr(new GeneralFile(it->path().string()))));
				return m_cachedFiles[filename];
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