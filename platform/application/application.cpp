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
		using Foundation::logger;
		using Foundation::LogLevel;
		using Foundation::FileSystemFactory;
		using Scene::SceneManager;
		Application::Application()
		{
			mFileSystem = FileSystemFactory::Instance()->CreateFileSystem();
			mWindowMgr = std::make_unique<WindowManager>();
			logger.Log(LogLevel::Info, "File system created!Current working directory:%s", mFileSystem->GetRoot().c_str());
			logger.Log(LogLevel::Info, "Application initialized successfully!");
		}

		Application::~Application()
		{
			SceneManager::Instance()->DestroyAll();
			if (mRenderer)
				mRenderer->ShutDown();
			mRenderer.reset();
			mWindowMgr.reset();
			logger.Log(LogLevel::Info, "Application quit.");
		}

		void Application::Start()
		{
			mWindow = CreateMainWindow();
			mRenderer = CreateRenderer();
			if (mRenderer)
				mRenderer->Start();
			//Create a simple scene here just for test
			auto scene = SceneManager::Instance()->CreateScene();
			auto cube = std::make_shared<Scene::Cube>(0.5);
			auto camera = scene->GetActiveCamera();
			camera->MoveTo(Render::Vector3f({2.0f, 2.0f, 2.0f}));
			camera->LookAt(Render::Vector3f({ 0.0f, 0.0f, 0.0f }));
			scene->AddDrawable(cube);
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
		}


	}
}