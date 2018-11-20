#pragma once
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "Singleton.h"
#include "IConfigManager.h"

namespace Lightning
{
	namespace Foundation
	{
		class ConfigManager : public IConfigManager, public Singleton<ConfigManager>
		{
		public:
			std::string GetConfigString(const std::string& node_path)override;
			const EngineConfig& GetConfig()const override{ return mConfig; }
		private:
			friend class Singleton<ConfigManager>;
			ConfigManager();
			EngineConfig mConfig;
			boost::property_tree::ptree mTree;
			static constexpr char* CONFIG_FILE_NAME = "config.xml";
		};
	}
}
