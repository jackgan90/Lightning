#include "Common.h"
#include "Application.h"
#include "Logger.h"
#include "IRenderer.h"
#include "ISceneManager.h"
//include primitives here just for simple scene construction
#include "IPrimitive.h"
#include "Transform.h"
#include "Loader.h"
#include "IPluginManager.h"
#include "IFoundationPlugin.h"
#include "IScenePlugin.h"
#include "IRenderPlugin.h"
#include "IWindowPlugin.h"
#include "tbb/task_scheduler_init.h"

namespace
{
	Lightning::Plugins::IWindowPlugin* windowPlugin = nullptr;
	Lightning::Plugins::IRenderPlugin* renderPlugin = nullptr;
	Lightning::Plugins::IScenePlugin* scenePlugin = nullptr;
}

namespace Lightning
{
	namespace Plugins
	{
		extern IPluginManager* gPluginMgr;
	}
	namespace App
	{
		using namespace Window;
		using Scene::ISceneManager;
		using Foundation::Math::EulerAnglef;
		using Foundation::Math::Quaternionf;
		using Foundation::Math::Vector3f;
		using Foundation::Math::Transformer;

		//For test only
		void GenerateSceneObjects(ISceneManager* sceneMgr, Plugins::IScenePlugin* scenePlugin)
		{
			auto scene = sceneMgr->GetForegroundScene();
			
			auto cube = scenePlugin->CreateCube(1.0f, 1.0f, 1.0f);
			Render::Color32 color;
			color.r = 0;
			color.g = 255;
			color.b = 0;
			color.a = 255;
			cube->SetColor(color);
			scene->AddDrawable(cube);
			cube->Release();
			auto renderer = renderPlugin->GetRenderer();
			auto device = renderer->GetDevice();
			struct test : Render::IShaderCallback
			{
				test(Scene::IPrimitive* p) : prim(p){}
				void Execute(Render::IShader* shader)override
				{	
					prim->SetShader(shader);
					delete this;
				}
				Scene::IPrimitive* prim;
			};
			
			device->CreateShaderFromFile(Render::ShaderType::VERTEX, "default.vs", new test(cube));


			//device->CreateShaderFromFile(Render::ShaderType::FRAGMENT, "texture_map.ps", new test(cube));
			/*
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
				p->SetWorldRotation(Transformer::RandomRotation());
				scene->AddDrawable(p);
				p->Release();
			}*/
		}
		//For test only end

		Application::Application() 
		: mExitCode(0)
		, mRunning(false)
		, mRenderer(nullptr)
		{
		}

		Application::~Application()
		{
		}

		void Application::Update()
		{
		}

		void Application::Start()
		{
			mRunning = true;
			auto foundationPlugin = Plugins::GetPlugin<Plugins::IFoundationPlugin>(Plugins::gPluginMgr, "Foundation");
			windowPlugin = Plugins::GetPlugin<Plugins::IWindowPlugin>(Plugins::gPluginMgr, "Window");
			renderPlugin = Plugins::GetPlugin<Plugins::IRenderPlugin>(Plugins::gPluginMgr, "Render");
			scenePlugin = Plugins::GetPlugin<Plugins::IScenePlugin>(Plugins::gPluginMgr, "Scene");
			mEventMgr = foundationPlugin->GetEventManager();
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
			auto window = windowPlugin->NewWindow();
			mRenderer = renderPlugin->CreateRenderer(window);
			mRenderer->Start();
			//Create a simple scene here just for test
			auto sceneMgr = scenePlugin->GetSceneManager();
			auto scene = sceneMgr->CreateScene();
			auto camera = scene->GetActiveCamera();
			camera->MoveTo(Render::Vector3f({2.0f, 2.0f, 2.0f}));
			camera->LookAt(Render::Vector3f({ 0.0f, 0.0f, 0.0f }));
			//camera->SetRotation(Quaternionf(EulerAnglef(3.14 + 0.0, 0, 0)));
			//camera->SetCameraType(Scene::CameraType::Orthographic);
			GenerateSceneObjects(sceneMgr, scenePlugin);
			//camera->RotateTowards(Render::Vector3f(0.0f, 1.0f, -1.0f));

			//End of scene creation
			RegisterWindowHandlers();
			window->Show(true);
			window->Release();
			LOG_INFO("Application start successfully!");
		}

		void Application::OnQuit(int exitCode)
		{
			mRunning = false;
			mExitCode = exitCode;
			auto sceneMgr = Plugins::GetPlugin<Plugins::IScenePlugin>(Plugins::gPluginMgr, "Scene")->GetSceneManager();
			sceneMgr->DestroyAllScenes();
			mRenderer->ShutDown();
			for (auto subscriberID : mSubscriberIDs)
			{
				mEventMgr->Unsubscribe(subscriberID);
			}
			LOG_INFO("Application quit.");
		}


		void Application::RegisterWindowHandlers()
		{
			auto subscriberId = WINDOW_MSG_CLASS_HANDLER(mEventMgr, WINDOW_IDLE_EVENT, OnWindowIdle);
			mSubscriberIDs.insert(subscriberId);
			subscriberId = mEventMgr->Subscribe(WINDOW_DESTROYED_EVENT, [this](const Foundation::IEvent& event) {
				OnQuit(int(static_cast<const WindowDestroyedEvent&>(event).exitCode));
			});
			mSubscriberIDs.insert(subscriberId);
		}

		void Application::OnWindowIdle(const Foundation::IEvent& event)
		{

		}
	}
}