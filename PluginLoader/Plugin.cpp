#include "Plugin.h"

namespace Lightning
{
	namespace Lib
	{
		Plugin::Plugin(const std::string& name): mName(name)
#ifdef LIGHTNING_WIN32
			, mHandle(NULL)
#endif
		{
			Load();
		}

		bool Plugin::IsLoaded()const
		{
#ifdef LIGHTNING_WIN32
			return mHandle != NULL;
#endif
		}

		bool Plugin::Load()
		{
			if (IsLoaded())
				return false;
#ifdef LIGHTNING_WIN32
			mHandle = ::LoadLibrary(GetFullName().c_str());
#endif
			return IsLoaded();
		}

		bool Plugin::Unload()
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

		std::string Plugin::GetFullName()const
		{
			return mName + PluginExtension;
		}
	}
}