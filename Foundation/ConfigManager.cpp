#include <boost/property_tree/exceptions.hpp>
#include <cstdlib>
#include "ConfigManager.h"
#include "Logger.h"

namespace Lightning
{
	namespace Foundation
	{
		namespace pt = boost::property_tree;

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
				mConfig.ResourceRoot = mTree.get<std::string>("Lightning.Resource.Root");
				mConfig.MSAAEnabled = mTree.get<bool>("Lightning.Render.MSAAEnable");
				mConfig.MSAASampleCount = mTree.get<unsigned>("Lightning.Render.MSAASampleCount");
				mConfig.ThreadCount = mTree.get<unsigned>("Lightning.General.Threads");
				for (auto& value : mTree.get_child("Lightning.Plugins"))
				{
					mConfig.Plugins.push_back(value.second.data());
				}
			}
			catch (pt::xml_parser_error e)
			{
				LOG_ERROR("Error in creating ConfigManager {0}", e.what());
				std::exit(EXIT_FAILURE);
			}

		}
	}
}