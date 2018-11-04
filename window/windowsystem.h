#pragma once
#include <memory>
#include <unordered_map>
#include <exception>
#include "windowexportdef.h"
#include "windowevents.h"
#include "logger.h"
#include "systempriority.h"
#include "ecs/event.h"
#include "ecs/system.h"
#include "ecs/entity.h"
#include "windowcomponent.h"

#define WINDOW_MSG_CLASS_HANDLER(EventType, Handler)\
Foundation::EventManager::Instance()->Subscribe<Window1::##EventType##>([&](const EventType& event){\
	this->Handler(event);\
})

namespace Lightning
{
	namespace Window1
	{
		using Foundation::System;
		using Foundation::WindowSystemPriority;
		using Foundation::Entity;
		using Foundation::EntityPtr;
		using Foundation::ComponentPtr;

		class WindowInitException : public std::exception
		{
		public:
			template<typename... Args>
			WindowInitException(const char*const w, const Args&... args) : exception(w)
			{
				LOG_ERROR(w, args...);
			}
		};

		class WindowEntity : public Entity
		{

		};

		class LIGHTNING_WINDOW_API WindowSystem : public System
		{
		public:
			virtual bool Show(bool show) = 0;
			virtual ~WindowSystem() = default;
			//Only left for backwards compatibility,will remove later once refactoring is over
			std::uint32_t GetWidth()const { return mComponent->width; }
			std::uint32_t GetHeight()const { return mComponent->height; }
		protected:
			WindowSystem();
			virtual void OnIdle();
			EntityPtr<WindowEntity> mEntity;
			ComponentPtr<WindowComponent> mComponent;
		};
		using SharedWindowPtr = std::shared_ptr<WindowSystem>;
	}
}
