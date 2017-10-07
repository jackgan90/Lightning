#pragma once
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "foundationexportdef.h"

namespace LightningGE
{
	namespace Foundation
	{
		struct EngineConfig
		{
			std::string ResourceRoot;		//the root directory of resources(shader,script,mesh etc)
			unsigned int SwapChainBufferCount;	//the buffer count of swapchain
			bool MSAAEnabled;				//is msaa enabled?
			unsigned int MSAASampleCount;	// msaa sample count
		};

		class LIGHTNINGGE_FOUNDATION_API ConfigManager
		{
		public:
			static const char* CONFIG_FILE_NAME;
			static ConfigManager* Instance();
			std::string GetConfigString(const std::string& node_path);
			const EngineConfig& GetConfig()const { return m_config; }
		private:
			ConfigManager();
			EngineConfig m_config;
			boost::property_tree::ptree m_tree;
		};
	}
}