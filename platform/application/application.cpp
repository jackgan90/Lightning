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
		using namespace WindowSystem;
		using Foundation::Math::EulerAnglef;
		using Foundation::Math::Quaternionf;
		using Foundation::Math::Vector3f;
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
			auto camera = scene->GetActiveCamera();
			camera->MoveTo(Render::Vector3f({0.0f, -2.0f, 0.0f}));
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

			auto cube = std::make_shared<Scene::Cube>();
			cube->SetWorldPosition(Vector3f(1.0, 0.0, 0.0));
			cube->SetColor(0xffff0000);
			scene->AddDrawable(cube);

			auto cylinder = std::make_shared<Scene::Cylinder>(2.0, 1.0);
			cylinder->SetWorldPosition(Vector3f(-1.0, 0.0, 0.0));
			//cylinder->SetWorldRotation(Quaternionf(EulerAnglef(0, 3.14 / 2, 0)));
			//auto rot = Quaternionf(EulerAnglef(0, 3.14 / 4, 0));
			//EulerAnglef euler;
			//rot.ToEulerAngle(euler);
			//rot *= Quaternionf(EulerAnglef(0, 3.14 / 4, 0));
			//rot.ToEulerAngle(euler);
			//cylinder->SetWorldRotation(rot);
			cylinder->SetColor(0xffffff00);
			scene->AddDrawable(cylinder);

			auto hemisphere = std::make_shared<Scene::Hemisphere>();
			hemisphere->SetWorldPosition(Vector3f(0, 0, 0));
			hemisphere->SetColor(0x5f00ff00);
			scene->AddDrawable(hemisphere);

			auto sphere = std::make_shared<Scene::Sphere>();
			sphere->SetWorldPosition(Vector3f(0, 1, 0));
			sphere->SetColor(0x4f0000ff);
			scene->AddDrawable(sphere);
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
			SceneManager::Instance()->Update();
			if (mTimer)
				mTimer->Tick();
			if(mRenderer)
				mRenderer->Render();
		}


	}
}