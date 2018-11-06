#include "library.h"

namespace Lightning
{
	namespace Lib
	{
		Library::Library(const std::string& name): mName(name)
#ifdef LIGHTNING_WIN32
			, mHandle(NULL)
#endif
		{
			Load();
		}

		bool Library::IsLoaded()const
		{
#ifdef LIGHTNING_WIN32
			return mHandle != NULL;
#endif
		}

		bool Library::Load()
		{
			if (IsLoaded())
				return false;
#ifdef LIGHTNING_WIN32
			mHandle = ::LoadLibrary(GetFullName().c_str());
#endif
			return IsLoaded();
		}

		bool Library::Unload()
		{
			if (!IsLoaded())
				return false;
#ifdef LIGHTNING_WIN32
			auto res = ::FreeLibrary(mHandle);
			if (res)
				mHandle = NULL;
#endif
			return !IsLoaded();
		}

		std::string Library::GetFullName()const
		{
			return mName + LibPostfix;
		}
	}
}