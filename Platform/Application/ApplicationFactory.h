#pragma once
#include <memory>
#include "Application.h"

namespace Lightning
{
	namespace App
	{
		class ApplicationFactory
		{
		public:
			static std::unique_ptr<Application> CreateApplication();
		};
	}
}
