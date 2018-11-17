#pragma once
#include "Plugin.h"
#include "Logger.h"

namespace Lightning
{
	namespace Plugins
	{
		class FoundationPlugin : public Plugin
		{
		public:
			virtual void InitLogger(const char* name, Foundation::Logger* logger) = 0;
		};
	}
}