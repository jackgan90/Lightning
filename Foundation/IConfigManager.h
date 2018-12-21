#pragma once
#include <type_traits>
#include "Container.h"

namespace Lightning
{
	namespace Foundation
	{
		struct EngineConfig
		{
			char* ResourceRoot;		//the root directory of resources(shader,script,mesh etc)
			bool MSAAEnabled;				//is msaa enabled?
			unsigned MSAASampleCount;	// msaa sample count
			unsigned ThreadCount;		// thread count used for the whole application(0 indicates determined by hardware)
			char** Plugins;				//engine plugins loaded on app start.
			unsigned PluginCount;
		};
		static_assert(std::is_pod<EngineConfig>::value, "EngineConfig is not a POD type.");

		struct IConfigManager
		{
			virtual ~IConfigManager() = default;
			virtual const EngineConfig& GetConfig()const = 0;
		};
	}
}