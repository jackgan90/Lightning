#pragma once
#include <unordered_map>
#include "singleton.h"
#include "allocatorid.h"
#include "basememoryallocator.h"

#define DECLARE_ALLOCATOR_INSTANCE_MEMBER(className) className m_Instance##className;

namespace Lightning
{
	namespace Foundation
	{
		class LIGHTNING_FOUNDATION_API AllocatorManager : public Lightning::Foundation::Singleton<AllocatorManager>
		{
		public:
			friend class Lightning::Foundation::Singleton<AllocatorManager>;
			IMemoryAllocator* GetAllocator(const AllocatorID allocatorId);
			~AllocatorManager(){}
		private:
			AllocatorManager(){}
			using AllocatorMap = std::unordered_map<AllocatorID, IMemoryAllocator*>;
		};	
	}

}
