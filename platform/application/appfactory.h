#pragma once
#include "platformexportdef.h"
#include "iapplication.h"

namespace LightningGE
{
	namespace App
	{
		class LIGHTNINGGE_PLATFORM_API AppFactory
		{
		public:
			static ApplicationPtr GetApp();
			static void Finalize();
		private:
			static ApplicationPtr s_app;
		};
	}
}