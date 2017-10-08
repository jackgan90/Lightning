#include "allocatormanager.h"

namespace memory
{
	AllocatorManager* AllocatorManager::s_instance = nullptr;
	const char * MultipleAllocatorManagerException::what() const
	{
		return "Multiple allocator manager detected!Application should only create one AllocatorManager instance!";
	}

	AllocatorManager * AllocatorManager::Instance()
	{
		return s_instance;
	}

	AllocatorManager::AllocatorManager()
	{
		if (s_instance)
		{
			throw MultipleAllocatorManagerException();
		}
		s_instance = this;
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