#pragma once
#include <memory>
#include <string>
#ifdef LIGHTNING_WIN32
#include <Windows.h>
#endif
#include "libexportdef.h"

namespace Lightning
{
	namespace Lib
	{
		class LIGHTNING_LIB_API Library
		{
		public:
			std::string GetName()const { return mName; }
			bool IsLoaded()const;
			bool Load();
			bool Unload();
			std::string GetFullName()const;
		private:
			friend class LibraryMgr;
			Library(const std::string& name);
			std::string mName;
#ifdef LIGHTNING_WIN32
			HMODULE mHandle;
			static constexpr char* LibPostfix = ".dll";
#endif
		};

		using LibraryPtr = std::shared_ptr<Library>;
	}
}