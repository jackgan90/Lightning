#include "Common.h"
#include "FileSystemFactory.h"
#include "Application.h"
#include "Logger.h"
#include "IRenderer.h"
#include "SceneManager.h"
//include primitives here just for simple scene construction
#include "Primitive.h"
#include "Transform.h"
#include "ConfigManager.h"
#include "Loader.h"
#include "SystemPriority.h"
#include "ECS/EventManager.h"
#include "tbb/task_scheduler_init.h"

namespace Lightning
{
	namespace App
	{
		using Foundation::FileSystemFactory;
		using Scene::SceneManager;
		using namespace Window;
		using Foundation::Math::EulerAnglef;
		using Foundation::Math::Quaternionf;
		using Foundation::Math::Vector3f;
		using Foundation::Math::Transform;
		using Foundation::EventManager;
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
			Loading::Loader::Instance()->SetFileSystem(mFileSystem);
			mWindow = CreateMainWindow();
			mRenderer = CreateRenderer();
			mRenderer->Start();
			EventManager::Instance()->Subscribe<WindowDestroyedEvent>([this](const WindowDestroyedEvent& event) {
				mRunning = false;
				mExitCode = int(event.exitCode);
				SceneManager::Instance()->DestroyAll();
				mRenderer->ShutDown();
				mRenderer.reset();
				Loading::Loader::Instance()->Finalize();
				LOG_INFO("Application quit.");
			});
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
			mWindow->Show(true);
			LOG_INFO("Application start successfully!");
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
			WINDOW_MSG_CLASS_HANDLER(WindowIdleEvent, OnWindowIdle);
		}

		void Application::OnWindowIdle(const WindowIdleEvent& event)
		{

		}
	}
}