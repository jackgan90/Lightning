#pragma once
#include "Application.h"

namespace Lightning
{
	namespace App
	{
		class ApplicationFactory
		{
		public:
			static Application* CreateApplication();
		};
	}
}
