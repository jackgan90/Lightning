#pragma once
#include "platformexportdef.h"

namespace LightningGE
{
	namespace App
	{
		class LIGHTNINGGE_PLATFORM_API IApplication
		{
		public:
			virtual bool Init() = 0;
			virtual bool Start() = 0;
			virtual int Run() = 0;
			virtual void Quit() = 0;
			virtual ~IApplication(){}
			static IApplication* getApp();
		};
	}
}
