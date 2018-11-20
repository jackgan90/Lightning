#pragma once
#include <string>

namespace Lightning
{
	namespace Foundation
	{
		struct EngineConfig
		{
			std::string ResourceRoot;		//the root directory of resources(shader,script,mesh etc)
			bool MSAAEnabled;				//is msaa enabled?
			unsigned MSAASampleCount;	// msaa sample count
			unsigned ThreadCount;		// thread count used for the whole application(0 indicates determined by hardware)
		};

		class IConfigManager
		{
		public:
			virtual ~IConfigManager() = default;
			virtual std::string GetConfigString(const std::string& node_path) = 0;
			virtual const EngineConfig& GetConfig()const = 0;
		};
	}
}