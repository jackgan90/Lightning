#pragma once
#include "Portable.h"

namespace Lightning
{
	namespace App
	{
		struct IApplication
		{
			virtual ~IApplication() = default;
			virtual void Tick() = 0;
			virtual void Start() = 0;
			virtual bool IsRunning() = 0;
			virtual int GetExitCode()const = 0;
		};
	}
}