#include "common.h"
#include "filesystemfactory.h"
#include "appsystem.h"
#include "logger.h"
#include "irenderer.h"
#include "scenemanager.h"
//include primitives here just for simple scene construction
#include "primitive.h"
#include "transform.h"
#include "configmanager.h"
#include "loader.h"
#include "systempriority.h"
#include "appcomponent.h"
#include "tbb/task_scheduler_init.h"

namespace Lightning
{
	namespace App
	{
		using Foundation::FileSystemFactory;
		using Scene::SceneManager;
		using namespace Window1;
		using Foundation::Math::EulerAnglef;
		using Foundation::Math::Quaternionf;
		using Foundation::Math::Vector3f;
		using Foundation::Math::Transform;
		using Foundation::EventManager;
		using Foundation::TimerManager;
		AppSystem::AppSystem() : System(Foundation::AppSystemPriority)
		{
			EventManager::Instance()->Subscribe<ComponentRemoved<AppComponent>>(
				[this](const ComponentRemoved<AppComponent>& component) {
				OnAppComponentRemoved(component);
			});
		}

		AppSystem::~AppSystem()
		{
		}

		void AppSystem::Update(const EntityPtr<Entity>& entity)
		{
			auto appComponent = entity->GetComponent<AppComponent>();
			if (appComponent)
			{
				bool started{ true };
				if (!mAppComponent)
				{
					mAppComponent = appComponent;
					mEntity = entity;
					started = false;
				}
				if (!appComponent->fileSystem)
				{
					appComponent->fileSystem = FileSystemFactory::Instance()->CreateFileSystem();
					Loading::Loader::Instance()->SetFileSystem(appComponent->fileSystem);
					LOG_INFO("File system created!Current working directory:{0}", 
						appComponent->fileSystem->GetRoot().c_str());
				}
				if (!started)
				{
					Start();
					LOG_INFO("Application initialized successfully!");
				}
			}
		}

		void AppSystem::Start()
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
			mAppComponent->window = CreateMainWindow();
			mAppComponent->renderer = CreateRenderer();
			mAppComponent->renderer->Start();
			mAppComponent->timer = TimerManager::Instance()->CreateTimer(10);
			mAppComponent->timer->Start();
			EventManager::Instance()->Subscribe<WindowDestroyedEvent>([this](const WindowDestroyedEvent& event) {
				mEntity->RemoveComponent<AppComponent>();
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
			mAppComponent->window->Show(true);
			LOG_INFO("Application start successfully!");
		}

		void AppSystem::GenerateSceneObjects()
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

		void AppSystem::RegisterWindowHandlers()
		{
			WINDOW_MSG_CLASS_HANDLER(WindowIdleEvent, OnWindowIdle);
		}

		void AppSystem::OnAppComponentRemoved(const ComponentRemoved<AppComponent>& event)
		{
			SceneManager::Instance()->DestroyAll();
			auto component = event.component;
			if (component->renderer)
			{
				component->renderer->ShutDown();
				component->renderer.reset();
			}
			Loading::Loader::Instance()->Finalize();
			LOG_INFO("Application quit.");
		}

		void AppSystem::OnWindowIdle(const WindowIdleEvent& event)
		{
			if (mAppComponent)
			{
				if (mAppComponent->timer)
					mAppComponent->timer->Tick();
				if (mAppComponent->renderer)
					mAppComponent->renderer->Render();
			}
		}


	}
}