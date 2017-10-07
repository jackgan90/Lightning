#pragma once
#include <memory>
#include "platformexportdef.h"
#include "iwindow.h"

namespace LightningGE
{
	namespace App
	{
		class IApplication;
		typedef std::shared_ptr<IApplication> ApplicationPtr;
		class LIGHTNINGGE_PLATFORM_API IApplication
		{
		public:
			virtual bool Init() = 0;
			virtual bool Start() = 0;
			virtual int Run() = 0;
			virtual void Quit() = 0;
			virtual ~IApplication(){}
		};

		class LIGHTNINGGE_PLATFORM_API Application : public IApplication
		{
		public:
			bool Start()override;
		protected:
			virtual void OnWindowIdle(const WindowSystem::WindowIdleParam& param){}
			virtual bool CreateMainWindow() = 0;
			virtual bool InitRenderContext() = 0;
			virtual void RegisterWindowHandlers();
			virtual WindowSystem::WindowPtr GetMainWindow() const = 0;
		};

	}
}
