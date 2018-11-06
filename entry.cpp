#include "ecs/entitymanager.h"
#include "ecs/systemmanager.h"
#include "ecs/eventmanager.h"
#include "appcomponent.h"
#ifdef LIGHTNING_WIN32
#include <Windows.h>	//for WinMain
#include "win32appsystem.h"
#endif
#include "logger.h" //for logging

using Lightning::Foundation::EntityManager;
using Lightning::Foundation::EventManager;
using Lightning::Foundation::SystemManager;
using Lightning::Foundation::ComponentPtr;
using Lightning::Foundation::Component;
using Lightning::App::AppEntity;
using Lightning::App::AppComponent;
namespace
{
	bool running{ true };
}
#ifdef LIGHTNING_WIN32
using Lightning::App::Win32AppSystem;
int APIENTRY WinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPTSTR    lpCmdLine,
					 int       nCmdShow)
#endif
{
	LOG_INFO("This is Lighting entry.");
	auto appEntity = EntityManager::Instance()->CreateEntity<AppEntity>();
	auto appComponent = appEntity->AddComponent<AppComponent>();
	int exitCode{ 0 };
	appEntity->RegisterComponentRemovedCallback([&](const ComponentPtr& comp) {
		exitCode = std::static_pointer_cast<AppComponent, Component>(comp)->exitCode;
		running = false;
	});
#ifdef LIGHTNING_WIN32
	auto app = SystemManager::Instance()->CreateSystem<Win32AppSystem>();
#endif
	while (running)
	{
		EntityManager::Instance()->Update();
	}
	SystemManager::Instance()->Reset();
	return exitCode;
}
