#include "common.h"
#include "filesystemfactory.h"
#include "iapplication.h"
#include "logger.h"
#include "irenderer.h"
#include "scenemanager.h"
//include primitives here just for simple scene construction
#include "primitive.h"


namespace Lightning
{
	namespace App
	{
		using Foundation::FileSystemFactory;
		using Scene::SceneManager;
		Application::Application():mTimer{nullptr}
		{
			mFileSystem = FileSystemFactory::Instance()->CreateFileSystem();
			mWindowMgr = std::make_unique<WindowManager>();
			LOG_INFO("File system created!Current working directory:%s", mFileSystem->GetRoot().c_str());
			LOG_INFO("Application initialized successfully!");
		}

		Application::~Application()
		{
			SceneManager::Instance()->DestroyAll();
			if (mRenderer)
				mRenderer->ShutDown();
			mRenderer.reset();
			mWindowMgr.reset();
			LOG_INFO("Application quit.");
		}

		void Application::Start()
		{
			mWindow = CreateMainWindow();
			mRenderer = CreateRenderer();
			if (mRenderer)
				mRenderer->Start();
			mTimer = TimerManager::Instance()->CreateTimer(10);
			mTimer->Start();
			//Create a simple scene here just for test
			auto scene = SceneManager::Instance()->CreateScene();
			//auto cube = std::make_shared<Scene::Cube>();
			//auto cylinder = std::make_shared<Scene::Cylinder>(2, 1);
			auto hemisphere = std::make_shared<Scene::Hemisphere>();
			auto sphere = std::make_shared<Scene::Sphere>();
			auto camera = scene->GetActiveCamera();
			camera->MoveTo(Render::Vector3f({2.0f, 2.0f, 2.0f}));
			camera->LookAt(Render::Vector3f({ 0.0f, 0.0f, 0.0f }));
			//scene->AddDrawable(cube);
			//scene->AddDrawable(cylinder);
			//scene->AddDrawable(hemisphere);
			scene->AddDrawable(sphere);
			//End of scene creation
			RegisterWindowHandlers();
		}

		void Application::RegisterWindowHandlers()
		{
			auto pWin = GetMainWindow();
			if (pWin)
			{
				pWin->RegisterWindowMessageHandler(WindowSystem::WindowMessage::IDLE, 
					[&](WindowSystem::WindowMessage msg, const WindowSystem::WindowMessageParam& param) 
					{this->OnWindowIdle(reinterpret_cast<const WindowSystem::WindowIdleParam&>(param)); });
			}
		}

		void Application::OnWindowIdle(const WindowSystem::WindowIdleParam& param)
		{
			SceneManager::Instance()->Update();
			if(mRenderer)
				mRenderer->Render();
			if (mTimer)
				mTimer->Tick();
		}


	}
}