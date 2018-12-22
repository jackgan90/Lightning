#pragma once
#include <memory>
#include "WindowEvents.h"
#include "IWindow.h"
#include "IApplication.h"
#include "IRenderer.h"
#include "ECS/IEventManager.h"
#include "Container.h"

namespace Lightning
{
	namespace App
	{
		using Render::IRenderer;
		
		class Application : public IApplication
		{
		public:
			Application();
			INTERFACECALL ~Application()override;
			void INTERFACECALL Update()override;
			void INTERFACECALL Start()override;
			bool INTERFACECALL IsRunning() override{ return mRunning; }
			int INTERFACECALL GetExitCode()const override{ return mExitCode; }
		protected:
			virtual void OnWindowIdle(const Foundation::IEvent& event);
			virtual void RegisterWindowHandlers();
			virtual void OnQuit(int exitCode);

			IRenderer* mRenderer;
			Foundation::IEventManager* mEventMgr;
			int mExitCode;
			bool mRunning;
			Foundation::Container::UnorderedSet<Foundation::EventSubscriberID> mSubscriberIDs;
		};

	}
}
