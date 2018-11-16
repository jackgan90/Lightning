#pragma once

namespace Lightning
{
	namespace App
	{
		class IApplication
		{
		public:
			virtual ~IApplication() = default;
			virtual void Update() = 0;
			virtual void Start() = 0;
			virtual bool IsRunning() = 0;
			virtual int GetExitCode()const = 0;
		};
	}
}