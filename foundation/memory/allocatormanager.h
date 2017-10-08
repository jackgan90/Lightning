#pragma once
#include <exception>
#include "memoryexportdef.h"
#include "basememoryallocator.h"

#define DECLARE_ALLOCATOR_INSTANCE_MEMBER(className, memberName) \
	public: \
		className* className##Instance(){return &m_##memberName;} \
	private: \
		className m_##memberName;

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
		AllocatorManager();
		~AllocatorManager();
	private:
		static AllocatorManager* s_instance;
		DECLARE_ALLOCATOR_INSTANCE_MEMBER(BaseMemoryAllocator, baseAllocator)
	};	

}