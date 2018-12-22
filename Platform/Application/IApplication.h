#pragma once
#include "Portable.h"

namespace Lightning
{
	namespace App
	{
		struct IApplication
		{
			virtual INTERFACECALL ~IApplication() = default;
			virtual void INTERFACECALL Update() = 0;
			virtual void INTERFACECALL Start() = 0;
			virtual bool INTERFACECALL IsRunning() = 0;
			virtual int INTERFACECALL GetExitCode()const = 0;
		};
	}
}