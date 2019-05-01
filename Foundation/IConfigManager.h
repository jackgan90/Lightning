#pragma once
#include <type_traits>
#include <string>
#include <vector>

namespace Lightning
{
	namespace Foundation
	{
		struct EngineConfig
		{
			std::string ResourceRoot;			//the root directory of resources(shader,script,mesh etc)
			bool MSAAEnabled;					//is MSAA enabled?
			unsigned MSAASampleCount;			// MSAA sample count
			unsigned ThreadCount;				// thread count used for the whole application(0 indicates determined by hardware)
			std::vector<std::string> Plugins;	//engine plugins loaded on app start.
		};

		struct IConfigManager
		{
			virtual ~IConfigManager() = default;
			virtual const EngineConfig& GetConfig()const = 0;
		};
	}
}