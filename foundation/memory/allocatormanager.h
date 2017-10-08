#pragma once
#include <exception>
#include <unordered_map>
#include "allocatorid.h"
#include "memoryexportdef.h"
#include "basememoryallocator.h"

#define DECLARE_ALLOCATOR_INSTANCE_MEMBER(className) className m_Instance##className;

namespace memory
{
	class NullAllocatorManagerException : std::exception
	{
	public:
		const char* what()const override;
	};
	class MultipleAllocatorManagerException : std::exception
	{
	public:
		const char* what()const override;
	};

	class MEMORY_API AllocatorManager
	{
	public:
		static AllocatorManager* Instance();
		static IMemoryAllocator* GetAllocator(const AllocatorID allocatorId);

		AllocatorManager();
		~AllocatorManager();
	private:
		typedef std::unordered_map<AllocatorID, IMemoryAllocator*> AllocatorMap;
		static AllocatorManager* s_instance;
		static AllocatorMap s_allocators;
		//warning!!!!
		//warning!!!!
		//warning!!!!
		//note : You have to put a line to initialize this member in allocatormanager.cpp!!!!
		DECLARE_ALLOCATOR_INSTANCE_MEMBER(BaseMemoryAllocator)
	};	

}