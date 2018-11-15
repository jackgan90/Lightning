#include "Engine.h"
#include "Application/ApplicationFactory.h"

namespace Lightning
{
	using App::ApplicationFactory;
	Engine::Engine()
	{

	}

	int Engine::Run()
	{
		mApplication = ApplicationFactory::CreateApplication();
		mApplication->Start();

		while (mApplication->IsRunning())
		{
			mApplication->Update();
		}

		return mApplication->GetExitCode();
	}
}