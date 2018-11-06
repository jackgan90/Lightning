#pragma once
#include <string>
#include "libexportdef.h"
#include "library.h"
#include "singleton.h"
#include "container.h"

namespace Lightning
{
	namespace Lib
	{
		using Foundation::container;
		class LIGHTNING_LIB_API LibraryMgr : Foundation::Singleton<LibraryMgr>
		{
		public:
			LibraryPtr Load(const std::string& libName);
			bool Unload(const std::string& libName);
			bool Unload(const LibraryPtr& library);
			void UnloadAll();
		private:
			friend class Foundation::Singleton<LibraryMgr>;
			LibraryMgr();
			container::unordered_map<std::string, LibraryPtr> mLibraries;
		};
	}
}