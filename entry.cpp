#include "ecs/entitymanager.h"
#include "ecs/systemmanager.h"
#include "appcomponent.h"
#ifdef LIGHTNING_WIN32
#include <Windows.h>	//for WinMain
#include "win32appsystem.h"
#endif
#include "logger.h" //for logging

using Lightning::Foundation::EntityManager;
using Lightning::Foundation::EventManager;
using Lightning::Foundation::SystemManager;
using Lightning::Foundation::ComponentRemoved;
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
#ifdef LIGHTNING_WIN32
	auto appEntity = EntityManager::Instance()->CreateEntity<AppEntity>();
	auto appComponent = appEntity->AddComponent<AppComponent>();
	auto app = SystemManager::Instance()->CreateSystem<Win32AppSystem>();
	EventManager::Instance()->Subscribe<ComponentRemoved<AppComponent>>(
		[&](const ComponentRemoved<AppComponent>& event) {
		running = false;
	});
#endif
	while (running)
	{
		EntityManager::Instance()->Update();
	}
	return 0;
}
