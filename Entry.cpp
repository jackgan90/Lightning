#ifdef LIGHTNING_WIN32
#include <Windows.h>	//for WinMain
#endif
#include "Logger.h" //for logging
#include "Engine.h"

#ifdef LIGHTNING_WIN32
int APIENTRY WinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPTSTR    lpCmdLine,
					 int       nCmdShow)
#endif
{
	LOG_INFO("This is Lighting entry.");
	//auto appEntity = EntityManager::Instance()->CreateEntity<Entity>();
	//auto appComponent = appEntity->AddComponent<AppComponent>();
	//int exitCode{ 0 };
	//Lightning::Foundation::EntityFuncID id = 
	//appEntity->RegisterCompRemovedFunc<AppComponent>([&](const std::shared_ptr<AppComponent>& comp) {
	//	exitCode = static_cast<int>(comp->exitCode);
	//	running = false;
	//	appEntity->UnregisterCompRemovedFunc(id);
	//});
//#ifdef LIGHTNING_WIN32
	//auto app = SystemManager::Instance()->CreateSystem<Win32AppSystem>();
//#endif
	//while (running)
	//{
	//	EntityManager::Instance()->Update();
	//}
	return Lightning::Engine::Instance()->Run();
}
