#pragma once
#include <memory>
#include "platformexportdef.h"
#include "ecs/entity.h"
#include "ecs/system.h"
#include "ecs/event.h"
#include "filesystem.h"
#include "timermanager.h"
#include "appcomponent.h"

namespace Lightning
{
	namespace Render
	{
		class IRenderer;
	}
	namespace App
	{
		using UniqueRendererPtr = std::unique_ptr<Render::IRenderer>;

		using Foundation::Entity;
		using Foundation::EntityPtr;
		using Foundation::ComponentPtr;
		using Foundation::ComponentRemoved;
		using Window::SharedWindowPtr;
		
		class LIGHTNING_PLATFORM_API AppEntity : public Foundation::Entity
		{

		};

		class LIGHTNING_PLATFORM_API AppSystem : public Foundation::System
		{
		public:
			AppSystem();
			virtual ~AppSystem();
			void Update(const EntityPtr<Entity>& entity)override;
		protected:
			virtual void Start();
			virtual void OnWindowIdle(const Window::WindowIdleEvent& event);
			virtual SharedWindowPtr CreateMainWindow() = 0;
			virtual UniqueRendererPtr CreateRenderer() = 0;
			virtual void RegisterWindowHandlers();
			virtual void OnAppComponentRemoved(const ComponentRemoved<AppComponent>& event);
			//For test
			void GenerateSceneObjects();

			ComponentPtr<AppComponent> mAppComponent;
			EntityPtr<Entity> mEntity;
		};

	}
}
