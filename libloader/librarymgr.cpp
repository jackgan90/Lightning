#include "librarymgr.h"

namespace Lightning
{
	namespace Lib
	{
		LibraryMgr::LibraryMgr()
		{

		}

		LibraryPtr LibraryMgr::Load(const std::string& libName)
		{
			auto it = mLibraries.find(libName);
			if (it != mLibraries.end())
			{
				if (!it->second->IsLoaded())
					it->second->Load();
				return it->second;
			}
			auto library = LibraryPtr(new Library(libName));
			mLibraries.emplace(libName, library);
			return library;
		}

		bool LibraryMgr::Unload(const std::string& libName)
		{
			auto it = mLibraries.find(libName);
			if (it != mLibraries.end())
			{
				return it->second->Unload();
			}
			return false;
		}

		bool LibraryMgr::Unload(const LibraryPtr& library)
		{
			if (library)
			{
				return library->Unload();
			}
			return false;
		}

		void LibraryMgr::UnloadAll()
		{
			for (auto it = mLibraries.begin();it != mLibraries.end();++it)
			{
				it->second->Unload();
			}
		}
	}
}