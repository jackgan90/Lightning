#include <boost/property_tree/exceptions.hpp>
#include <cstdlib>
#include "ConfigManager.h"
#include "Logger.h"

namespace Lightning
{
	namespace Foundation
	{
		namespace pt = boost::property_tree;

		ConfigManager::~ConfigManager()
		{
			delete[] mConfig.ResourceRoot;
			for (unsigned i = 0;i < mConfig.PluginCount;++i)
			{
				delete[] mConfig.Plugins[i];
			}
			delete[] mConfig.Plugins;
		}

		ConfigManager::ConfigManager()
		{
			//TODO : resolve exception
			try
			{
				pt::read_xml(CONFIG_FILE_NAME, mTree);
				auto resourceRoot = mTree.get<std::string>("Lightning.Resource.Root");
				mConfig.ResourceRoot = new char[resourceRoot.length() + 1];
#ifdef _MSC_VER
				strcpy_s(mConfig.ResourceRoot, resourceRoot.length() + 1, resourceRoot.c_str());
#else
				std::strcpy(mConfig.ResourceRoot, resourceRoot.c_str());
#endif
				mConfig.MSAAEnabled = mTree.get<bool>("Lightning.Render.MSAAEnable");
				mConfig.MSAASampleCount = mTree.get<unsigned>("Lightning.Render.MSAASampleCount");
				mConfig.ThreadCount = mTree.get<unsigned>("Lightning.General.Threads");
				Container::Vector<std::string> plugins;
				for (auto& value : mTree.get_child("Lightning.Plugins"))
				{
					plugins.push_back(value.second.data());
				}
				mConfig.PluginCount = unsigned(plugins.size());
				mConfig.Plugins = new char*[mConfig.PluginCount];
				for (unsigned i = 0;i < mConfig.PluginCount;++i)
				{
					const auto& pluginName = plugins[i];
					auto bufferSize = pluginName.length() + 1;
					mConfig.Plugins[i] = new char[bufferSize];
#ifdef _MSC_VER
					strcpy_s(mConfig.Plugins[i], bufferSize, pluginName.c_str());
#else
					std::strcpy(mConfig.Plugins[i], pluginName.c_str());
#endif
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