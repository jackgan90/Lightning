#include "allocatormanager.h"

#define ALLOCATOR_MAP_INITIALIZE(className) s_allocators[className::ClassID] = &m_Instance##className;


namespace memory
{
	AllocatorManager* AllocatorManager::s_instance = nullptr;
	AllocatorManager::AllocatorMap AllocatorManager::s_allocators;
	const char * MultipleAllocatorManagerException::what() const
	{
		return "Multiple allocator manager detected!Application should only create one AllocatorManager instance!";
	}

	AllocatorManager * AllocatorManager::Instance()
	{
		return s_instance;
	}

	IMemoryAllocator * AllocatorManager::GetAllocator(const AllocatorID allocatorId)
	{
		return s_allocators[allocatorId];
	}

	AllocatorManager::AllocatorManager()
	{
		if (s_instance)
		{
			throw MultipleAllocatorManagerException();
		}
		s_instance = this;
		//remember to declare the allocator type in allocatormanager.h
		//the declaration and initialization must exactly match
		ALLOCATOR_MAP_INITIALIZE(BaseMemoryAllocator)
	}
	AllocatorManager::~AllocatorManager()
	{
		s_instance = nullptr;
	}
	const char * NullAllocatorManagerException::what() const
	{
		return "You must create an AllocatorManager object to use the memory system!";
	}
}