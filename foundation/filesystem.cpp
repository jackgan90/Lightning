#include "filesystem.h"
#include <boost/filesystem.hpp>

using namespace boost::filesystem;
using std::string;
namespace LightningGE
{
	namespace Foundation
	{
		GeneralFile::GeneralFile()
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

		GeneralFileSystem::GeneralFileSystem()
		{
			path p = boost::filesystem::current_path();
			m_root = p.string();
		}
		
		GeneralFileSystem::~GeneralFileSystem()
		{

		}

		IFile* GeneralFileSystem::FindFile(std::string path)
		{
			return nullptr;
		}

		bool GeneralFileSystem::SetRoot(std::string root_path)
		{
			return true;
		}

	}
}