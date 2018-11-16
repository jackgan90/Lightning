#include "Common.h"
#include "FileSystemFactory.h"
#include "Application.h"
#include "Logger.h"
#include "IRenderer.h"
#include "ISceneManager.h"
//include primitives here just for simple scene construction
#include "IPrimitive.h"
#include "Transform.h"
#include "ConfigManager.h"
#include "Loader.h"
#include "PluginSystem/IPluginMgr.h"
#include "LoaderPlugin.h"
#include "ScenePlugin.h"
#include "ECS/EventManager.h"
#include "tbb/task_scheduler_init.h"

namespace Lightning
{
	namespace Plugins
	{
		extern IPluginMgr* gPluginMgr;
	}
	namespace App
	{
		using Foundation::FileSystemFactory;
		using Scene::ISceneManager;
		using namespace Window;
		using Foundation::Math::EulerAnglef;
		using Foundation::Math::Quaternionf;
		using Foundation::Math::Vector3f;
		using Foundation::Math::Transform;
		using Foundation::EventManager;

		//For test only
		void GenerateSceneObjects(ISceneManager* sceneMgr, Plugins::ScenePlugin* scenePlugin)
		{
			auto scene = sceneMgr->GetForegroundScene();
			static std::random_device rd;
			static std::mt19937 mt(rd());
			static std::uniform_real_distribution<float> rDist(-2, 2);
			static std::uniform_int_distribution<int> dist(0, 3);
			static std::uniform_int_distribution<int> cDist(0, 255);
			for (auto i = 0;i < 200;++i)
			{
				Scene::IPrimitive* p{ nullptr };
				switch (dist(mt))
				{
				case 0:
					p = scenePlugin->CreateCube(1.0f, 1.0f, 1.0f);
					break;
				case 1:
					p = scenePlugin->CreateCylinder(2.0f, 1.0f);
					break;
				case 2:
					p = scenePlugin->CreateHemisphere(1.0f);
					break;
				case 3:
					p = scenePlugin->CreateSphere(1.0f);
					break;
				default:
					break;
				}
				Vector3f pos;
				pos.x = rDist(mt);
				pos.y = rDist(mt);
				pos.z = rDist(mt);
				p->SetWorldPosition(pos);
				Render::Color32 color;
				color.a = 255;
				color.r = cDist(mt);
				color.g = cDist(mt);
				color.b = cDist(mt);
				p->SetColor(color);
				p->SetWorldRotation(Transform::RandomRotation());
				scene->AddDrawable(p);
			}
		}
		//For test only end

		Application::Application() : mExitCode(0), mRunning(false)
		{
		}

		Application::~Application()
		{
		}

		void Application::Update()
		{
			if (mWindow)
				mWindow->Update();
			if (mRenderer)
				mRenderer->Render();
		}

		void Application::Start()
		{
			mRunning = true;
			auto loaderPlugin = Plugins::gPluginMgr->Load<Plugins::LoaderPlugin>("Loader");
			auto scenePlugin = Plugins::gPluginMgr->Load<Plugins::ScenePlugin>("Scene");
			static tbb::task_scheduler_init init(tbb::task_scheduler_init::deferred);
			auto threadCount = Foundation::ConfigManager::Instance()->GetConfig().ThreadCount;
			if (threadCount == 0)
			{
				init.initialize();
			}
			else
			{
				init.initialize(threadCount);
			}
			mFileSystem = FileSystemFactory::Instance()->CreateFileSystem();
			LOG_INFO("File system created!Current working directory:{0}", mFileSystem->GetRoot().c_str());
			loaderPlugin->GetLoader()->SetFileSystem(mFileSystem.get());
			mWindow = CreateMainWindow();
			mRenderer = CreateRenderer();
			mRenderer->Start();
			EventManager::Instance()->Subscribe<WindowDestroyedEvent>([this](const WindowDestroyedEvent& event) {
				OnQuit(int(event.exitCode));
			});
			//Create a simple scene here just for test
			auto sceneMgr = scenePlugin->GetSceneManager();
			auto scene = sceneMgr->CreateScene();
			auto camera = scene->GetActiveCamera();
			camera->MoveTo(Render::Vector3f({0.0f, 0.0f, 2.0f}));
			camera->LookAt(Render::Vector3f({ 0.0f, 0.0f, 0.0f }));
			//camera->SetRotation(Quaternionf(EulerAnglef(3.14 + 0.0, 0, 0)));
			//camera->SetCameraType(Scene::CameraType::Orthographic);
			GenerateSceneObjects(sceneMgr, scenePlugin);
			//camera->RotateTowards(Render::Vector3f(0.0f, 1.0f, -1.0f));

			//End of scene creation
			RegisterWindowHandlers();
			mWindow->Show(true);
			LOG_INFO("Application start successfully!");
		}

		void Application::OnQuit(int exitCode)
		{
			mRunning = false;
			mExitCode = exitCode;
			auto sceneMgr = Plugins::gPluginMgr->GetPlugin<Plugins::ScenePlugin>("Scene")->GetSceneManager();
			sceneMgr->DestroyAllScenes();
			mRenderer->ShutDown();
			mRenderer.reset();
			Plugins::gPluginMgr->Unload("Loader");
			Plugins::gPluginMgr->Unload("Scene");
			LOG_INFO("Application quit.");
		}


		void Application::RegisterWindowHandlers()
		{
			WINDOW_MSG_CLASS_HANDLER(WindowIdleEvent, OnWindowIdle);
		}

		void Application::OnWindowIdle(const WindowIdleEvent& event)
		{

		}
	}
}