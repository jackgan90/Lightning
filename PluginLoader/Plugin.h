#pragma once
#include <memory>
#include <string>
#ifdef LIGHTNING_WIN32
#include <Windows.h>
#endif
#include "PluginExportDef.h"

namespace Lightning
{
	namespace Lib
	{
		class LIGHTNING_PLUGIN_API Plugin
		{
		public:
			std::string GetName()const { return mName; }
			bool IsLoaded()const;
			bool Load();
			bool Unload();
			std::string GetFullName()const;
		private:
			friend class PluginMgr;
			Plugin(const std::string& name);
			std::string mName;
#ifdef LIGHTNING_WIN32
			HMODULE mHandle;
			static constexpr char* PluginExtension = ".dll";
#endif
		};

		using PluginPtr = std::shared_ptr<Plugin>;
	}
}