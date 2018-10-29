#include <boost/property_tree/exceptions.hpp>
#include <cstdlib>
#include "configmanager.h"
#include "logger.h"

namespace Lightning
{
	namespace Foundation
	{
		namespace pt = boost::property_tree;
		const char* ConfigManager::CONFIG_FILE_NAME = "config.xml";

		std::string ConfigManager::GetConfigString(const std::string& node_path)
		{
			return mTree.get<std::string>(node_path);
		}

		ConfigManager::ConfigManager()
		{
			//TODO : resolve exception
			try
			{
				pt::read_xml(CONFIG_FILE_NAME, mTree);
				mConfig.ResourceRoot = mTree.get<std::string>("lightning_game_engine_configuration.resource.root");
				mConfig.MSAAEnabled = mTree.get<bool>("lightning_game_engine_configuration.render.msaa_enable");
				mConfig.MSAASampleCount = mTree.get<unsigned>("lightning_game_engine_configuration.render.msaa_sample_count");
				mConfig.ThreadCount = mTree.get<unsigned>("lightning_game_engine_configuration.general.threads");
			}
			catch (pt::xml_parser_error e)
			{
				LOG_ERROR("Error in creating ConfigManager {0}", e.what());
				std::exit(EXIT_FAILURE);
			}

		}
	}
}