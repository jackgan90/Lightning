#pragma once
#include <memory>
#include "platformexportdef.h"
#include "iwindow.h"
#include "memorysystem.h"
#include "singleton.h"

namespace LightningGE
{
	namespace App
	{
		template<typename Derived>
		class LIGHTNINGGE_PLATFORM_API IApplication : public Foundation::Singleton<Derived>
		{
		public:
			virtual bool Init() = 0;
			virtual bool Start() = 0;
			virtual int Run() = 0;
			virtual void Quit() = 0;
			virtual ~IApplication(){}
		};

		template<typename Derived>
		class LIGHTNINGGE_PLATFORM_API Application : public IApplication<Derived>
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

#ifdef LIGHTNINGGE_PLATFORM_EXPORT
		template<typename Derived>
		bool Application<Derived>::Start()
		{
			bool result = true;
			result &= CreateMainWindow();
			if(result)
				result &= InitRenderContext();
			if (result)
				RegisterWindowHandlers();
			return result;
		}

		template<typename Derived>
		void Application<Derived>::RegisterWindowHandlers()
		{
			auto pWin = GetMainWindow();
			if (pWin)
			{
				pWin->RegisterWindowMessageHandler(WindowSystem::MESSAGE_IDLE, 
					[&](WindowSystem::WindowMessage msg, const WindowSystem::WindowMessageParam& param) 
					{this->OnWindowIdle(reinterpret_cast<const WindowSystem::WindowIdleParam&>(param)); });
			}
		}
#endif

	}
}
