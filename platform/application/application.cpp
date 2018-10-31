#include "common.h"
#include "filesystemfactory.h"
#include "iapplication.h"
#include "logger.h"
#include "irenderer.h"
#include "scenemanager.h"
//include primitives here just for simple scene construction
#include "primitive.h"
#include "transform.h"
#include "configmanager.h"
#include "loadermgr.h"
#include "tbb/task_scheduler_init.h"

namespace Lightning
{
	namespace App
	{
		using Foundation::FileSystemFactory;
		using Scene::SceneManager;
		using namespace WindowSystem;
		using Foundation::Math::EulerAnglef;
		using Foundation::Math::Quaternionf;
		using Foundation::Math::Vector3f;
		using Foundation::Math::Transform;
		Application::Application():mTimer{nullptr}
		{
			mFileSystem = FileSystemFactory::Instance()->CreateFileSystem();
			mWindowMgr = std::make_unique<WindowManager>();
			LOG_INFO("File system created!Current working directory:{0}", mFileSystem->GetRoot().c_str());
			LOG_INFO("Application initialized successfully!");
		}

		Application::~Application()
		{
			SceneManager::Instance()->DestroyAll();
			if (mRenderer)
				mRenderer->ShutDown();
			Loader::LoaderMgr::Instance()->Finalize();
			mRenderer.reset();
			mWindowMgr.reset();
			LOG_INFO("Application quit.");
		}

		void Application::Start()
		{
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
			Loader::LoaderMgr::Instance()->SetFileSystem(mFileSystem);
			mWindow = CreateMainWindow();
			mRenderer = CreateRenderer();
			mRenderer->Start();
			mTimer = TimerManager::Instance()->CreateTimer(10);
			mTimer->Start();
			//Create a simple scene here just for test
			auto scene = SceneManager::Instance()->CreateScene();
			auto camera = scene->GetActiveCamera();
			camera->MoveTo(Render::Vector3f({0.0f, 0.0f, 2.0f}));
			camera->LookAt(Render::Vector3f({ 0.0f, 0.0f, 0.0f }));
			//camera->SetRotation(Quaternionf(EulerAnglef(3.14 + 0.0, 0, 0)));
			//camera->SetCameraType(Scene::CameraType::Orthographic);
			GenerateSceneObjects();
			//camera->RotateTowards(Render::Vector3f(0.0f, 1.0f, -1.0f));

			//End of scene creation
			RegisterWindowHandlers();
		}

		void Application::GenerateSceneObjects()
		{
			auto scene = SceneManager::Instance()->GetForegroundScene();
			static std::random_device rd;
			static std::mt19937 mt(rd());
			static std::uniform_real_distribution<float> rDist(-2, 2);
			static std::uniform_int_distribution<int> dist(0, 3);
			static std::uniform_int_distribution<int> cDist(0, 255);
			for (auto i = 0;i < 200;++i)
			{
				std::shared_ptr<Scene::Primitive> p;
				switch (dist(mt))
				{
				case 0:
					p = std::make_shared<Scene::Cube>();
					break;
				case 1:
					p = std::make_shared<Scene::Cylinder>(2.0f, 1.0f);
					break;
				case 2:
					p = std::make_shared<Scene::Hemisphere>();
					break;
				case 3:
					p = std::make_shared<Scene::Sphere>();
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

		void Application::RegisterWindowHandlers()
		{
			auto window = GetMainWindow();
			if (window)
			{
				WINDOW_MSG_CLASS_HANDLER(window, WindowMessage::IDLE, WindowIdleParam, OnWindowIdle);
			}
		}

		void Application::OnWindowIdle(const WindowIdleParam& param)
		{
			if (mTimer)
				mTimer->Tick();
			if(mRenderer)
				mRenderer->Render();
		}


	}
}