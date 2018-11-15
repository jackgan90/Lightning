#pragma once
#include <memory>
#include "PlatformExportDef.h"
#include "Application/Application.h"

namespace Lightning
{
	namespace App
	{
		class LIGHTNING_PLATFORM_API ApplicationFactory
		{
		public:
			static std::unique_ptr<Application> CreateApplication();
		};
	}
}
