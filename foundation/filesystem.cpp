#include "filesystem.h"

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