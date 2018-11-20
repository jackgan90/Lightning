#include "Common.h"
#include "Application.h"
#include "Logger.h"
#include "IRenderer.h"
#include "ISceneManager.h"
//include primitives here just for simple scene construction
#include "IPrimitive.h"
#include "Transform.h"
#include "Loader.h"
#include "IPluginMgr.h"
#include "FoundationPlugin.h"
#include "LoaderPlugin.h"
#include "ScenePlugin.h"
#include "RenderPlugin.h"
#include "WindowPlugin.h"
#include "tbb/task_scheduler_init.h"

namespace
{
	Lightning::Plugins::FoundationPlugin* foundationPlugin = nullptr;
	Lightning::Plugins::LoaderPlugin* loaderPlugin = nullptr;
	Lightning::Plugins::WindowPlugin* windowPlugin = nullptr;
	Lightning::Plugins::RenderPlugin* renderPlugin = nullptr;
	Lightning::Plugins::ScenePlugin* scenePlugin = nullptr;
}

namespace Lightning
{
	namespace Plugins
	{
		extern IPluginMgr* gPluginMgr;
	}
	namespace App
	{
		using namespace Window;
		using Scene::ISceneManager;
		using Foundation::Math::EulerAnglef;
		using Foundation::Math::Quaternionf;
		using Foundation::Math::Vector3f;
		using Foundation::Math::Transform;

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
				p->Release();
			}
		}
		//For test only end

		Application::Application() 
		: mExitCode(0)
		, mRunning(false)
		, mFileSystem(nullptr)
		, mWindow(nullptr)
		, mRenderer(nullptr)
		{
		}

		Application::~Application()
		{
		}

		void Application::Update()
		{
			if (mRunning && mWindow)
				mWindow->Update();
			if (mRunning && mRenderer)
				mRenderer->Render();
		}

		void Application::Start()
		{
			mRunning = true;
			foundationPlugin = Plugins::gPluginMgr->Load<Plugins::FoundationPlugin>("Foundation");
			loaderPlugin = Plugins::gPluginMgr->Load<Plugins::LoaderPlugin>("Loader");
			windowPlugin = Plugins::gPluginMgr->Load<Plugins::WindowPlugin>("Window");
			renderPlugin = Plugins::gPluginMgr->Load<Plugins::RenderPlugin>("Render");
			scenePlugin = Plugins::gPluginMgr->Load<Plugins::ScenePlugin>("Scene");
			static tbb::task_scheduler_init init(tbb::task_scheduler_init::deferred);
			auto threadCount = foundationPlugin->GetConfigManager()->GetConfig().ThreadCount;
			if (threadCount == 0)
			{
				init.initialize();
			}
			else
			{
				init.initialize(threadCount);
			}
			mFileSystem = foundationPlugin->CreateFileSystem();
			LOG_INFO("File system created!Current working directory:{0}", mFileSystem->GetRoot().c_str());
			loaderPlugin->GetLoader()->SetFileSystem(mFileSystem);
			mWindow = windowPlugin->NewWindow();
			mRenderer = renderPlugin->CreateRenderer(mWindow);
			mRenderer->Start();
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
			if (mFileSystem)
				mFileSystem->Release();
			if (mWindow)
				mWindow->Release();
			auto eventMgr = foundationPlugin->GetEventManager();
			for (auto subscriberID : mSubscriberIDs)
			{
				eventMgr->Unsubscribe(subscriberID);
			}
			Plugins::gPluginMgr->Unload("Scene");
			Plugins::gPluginMgr->Unload("Render");
			//Don't unload window here, cuz OnQuit is called from WndProc,unload here will cause process crash
			//Plugins::gPluginMgr->Unload("Window");
			Plugins::gPluginMgr->Unload("Loader");
			Plugins::gPluginMgr->Unload("Foundation");
			LOG_INFO("Application quit.");
		}


		void Application::RegisterWindowHandlers()
		{
			auto eventMgr = foundationPlugin->GetEventManager();
			auto subscriberId = WINDOW_MSG_CLASS_HANDLER(eventMgr, WINDOW_IDLE_EVENT, OnWindowIdle);
			mSubscriberIDs.insert(subscriberId);
			subscriberId = foundationPlugin->GetEventManager()->Subscribe(WINDOW_DESTROYED_EVENT, [this](const Foundation::IEvent& event) {
				OnQuit(int(static_cast<const WindowDestroyedEvent&>(event).exitCode));
			});
			mSubscriberIDs.insert(subscriberId);
		}

		void Application::OnWindowIdle(const Foundation::IEvent& event)
		{

		}
	}
}