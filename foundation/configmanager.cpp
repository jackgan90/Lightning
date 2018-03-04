#include <boost/property_tree/exceptions.hpp>
#include <cstdlib>
#include "configmanager.h"
#include "logger.h"

namespace LightningGE
{
	namespace Foundation
	{
		namespace pt = boost::property_tree;
		const char* ConfigManager::CONFIG_FILE_NAME = "config.xml";
		extern Logger logger;

		std::string ConfigManager::GetConfigString(const std::string& node_path)
		{
			return m_tree.get<std::string>(node_path);
		}

		ConfigManager::ConfigManager()
		{
			//TODO : resolve exception
			try
			{
				pt::read_xml(CONFIG_FILE_NAME, m_tree);
				m_config.ResourceRoot = m_tree.get<std::string>("lightning_game_engine_configuration.resource.root");
				m_config.MSAAEnabled = m_tree.get<bool>("lightning_game_engine_configuration.render.msaa_enable");
				m_config.MSAASampleCount = m_tree.get<unsigned int>("lightning_game_engine_configuration.render.msaa_sample_count");
			}
			catch (pt::xml_parser_error e)
			{
				logger.Log(LogLevel::Error, "Error in creating ConfigManager %s", e.what());
				std::exit(EXIT_FAILURE);
			}

		}
	}
}