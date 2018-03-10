#include "common.h"
#include "filesystemfactory.h"
#include "iapplication.h"
#include "logger.h"
#include "irenderer.h"
#include "scenemanager.h"
//include primitives here just for simple scene construction
#include "primitive.h"


namespace LightningGE
{
	namespace App
	{
		using Foundation::logger;
		using Foundation::LogLevel;
		using Foundation::FileSystemFactory;
		using Scene::SceneManager;
		Application::Application()
		{
			m_fs = FileSystemFactory::Instance()->CreateFileSystem();
			m_windowMgr = std::make_unique<WindowManager>();
			logger.Log(LogLevel::Info, "File system created!Current working directory:%s", m_fs->GetRoot().c_str());
			logger.Log(LogLevel::Info, "Application initialized successfully!");
		}

		Application::~Application()
		{
			SceneManager::Instance()->DestroyAll();
			m_renderer.reset();
			m_windowMgr.reset();
			logger.Log(LogLevel::Info, "Application quit.");
		}

		void Application::Start()
		{
			m_window = CreateMainWindow();
			m_renderer = CreateRenderer();
			//Create a simple scene here just for test
			auto scene = SceneManager::Instance()->CreateScene();
			auto cube = std::make_shared<Scene::Cube>(0.5);
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
			if(m_renderer)
				m_renderer->Render();
		}


	}
}