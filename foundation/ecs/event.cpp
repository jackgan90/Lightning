#pragma once
#include "event.h"

namespace Lightning
{
	namespace Foundation
	{
		EventSubscriberID EventManager::sCurrentID = EventSubscriberID(0);
		container::unordered_map<EventTypeID, EventManager::EventSubscribers> EventManager::sSubscribers;
	}
}
