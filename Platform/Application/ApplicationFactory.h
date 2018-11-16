#pragma once
#include <memory>
#include "IApplication.h"

namespace Lightning
{
	namespace App
	{
		class ApplicationFactory
		{
		public:
			static std::unique_ptr<IApplication> CreateApplication();
		};
	}
}
