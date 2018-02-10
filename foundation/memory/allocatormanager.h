#pragma once
#include <unordered_map>
#include "singleton.h"
#include "allocatorid.h"
#include "basememoryallocator.h"

#define DECLARE_ALLOCATOR_INSTANCE_MEMBER(className) className m_Instance##className;

namespace LightningGE
{
	namespace Foundation
	{
		class LIGHTNINGGE_FOUNDATION_API AllocatorManager : public LightningGE::Foundation::Singleton<AllocatorManager>
		{
		public:
			friend class LightningGE::Foundation::Singleton<AllocatorManager>;
			IMemoryAllocator* GetAllocator(const AllocatorID allocatorId);
			~AllocatorManager(){}
		private:
			AllocatorManager(){}
			using AllocatorMap = std::unordered_map<AllocatorID, IMemoryAllocator*>;
		};	
	}

}