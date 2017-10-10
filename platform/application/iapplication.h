#pragma once
#include <memory>
#include "platformexportdef.h"
#include "filesystem.h"
#include "windowmanager.h"
#include "memorysystem.h"
#include "singleton.h"

namespace LightningGE
{
	namespace App
	{
		using Foundation::FileSystemPtr;
		using WindowSystem::WindowManager;
		class LIGHTNINGGE_PLATFORM_API IApplication
		{
		public:
			virtual bool Init() = 0;
			virtual void Start() = 0;
			virtual int Run() = 0;
			virtual void Quit() = 0;
			virtual WindowManager* GetWindowManager()const = 0;
			virtual ~IApplication(){}
		};

		class LIGHTNINGGE_PLATFORM_API Application : public IApplication
		{
		public:
			void Start()override;
		protected:
			virtual void OnWindowIdle(const WindowSystem::WindowIdleParam& param){}
			virtual bool CreateMainWindow() = 0;
			virtual void InitRenderContext() = 0;
			virtual void RegisterWindowHandlers();
			virtual WindowSystem::WindowPtr GetMainWindow() const = 0;
		};

	}
}
