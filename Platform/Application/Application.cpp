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
			color.r = 255;
			color.g = 255;
			color.b = 255;
			color.a = 255;
			cube->SetColor(color);
			//scene->AddDrawable(cube);
			//cube->Release();
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

			struct test1 : Render::ITextureCallback
			{
				test1(Scene::IScene* _scene, Scene::IPrimitive* p) : scene(_scene), prim(p){}
				void Execute(Render::ITexture* texture)override
				{
					prim->SetTexture("tex", texture);
					scene->AddDrawable(prim);
					prim->Release();
					delete this;
				}
				Scene::IPrimitive* prim;
				Scene::IScene* scene;
			};
			
			device->CreateShaderFromFile(Render::ShaderType::VERTEX, "texture_map.vs", new test(cube));
			device->CreateShaderFromFile(Render::ShaderType::FRAGMENT, "texture_map.ps", new test(cube));

			device->CreateTextureFromFile("Purged_One_Human_Jumper.jpg", new test1(scene, cube));
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
			mWindow = windowPlugin->CreateWindow();
			mWindow->RegisterEventReceiver(this);
			mRenderer = renderPlugin->CreateRenderer(mWindow);
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
			mWindow->Show(true);
			LOG_INFO("Application start successfully!");
		}

		void Application::OnQuit(int exitCode)
		{
			mWindow->UnregisterEventReceiver(this);
			mWindow->Release();
			mRunning = false;
			mExitCode = exitCode;
			auto sceneMgr = Plugins::GetPlugin<Plugins::IScenePlugin>(Plugins::gPluginMgr, "Scene")->GetSceneManager();
			sceneMgr->DestroyAllScenes();
			mRenderer->ShutDown();
			LOG_INFO("Application quit.");
		}

		void Application::OnWindowCreated(IWindow* window)
		{

		}

		void Application::OnWindowDestroy(IWindow* window, int exitCode)
		{
			OnQuit(mExitCode);
		}

		void Application::OnWindowIdle(IWindow* window)
		{

		}

		void Application::OnWindowResize(IWindow* window, std::size_t width, std::size_t height)
		{

		}

		void Application::OnWindowMouseWheel(IWindow* window, int delta, bool isVertical)
		{

		}

		void Application::OnWindowKeyDown(IWindow* window, VirtualKeyCode keyCode)
		{

		}

		void Application::OnWindowMouseDown(IWindow* window, VirtualKeyCode keyCode, std::size_t x, std::size_t y)
		{

		}

		void Application::OnWindowMouseMove(IWindow* window, VirtualKeyCode keyCode, std::size_t x, std::size_t y)
		{

		}
	}
}