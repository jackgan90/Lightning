#pragma once
#include "imemoryallocator.h"
//#define ENABLE_MEMORY_LOG

namespace LightningGE
{
	namespace Foundation
	{
		//The memory layout of the memory store is like this:user mem0, memoryNode, user mem1, memoryNode, user mem2, memoryNode ...
		class LIGHTNINGGE_FOUNDATION_API StackAllocator : public IMemoryAllocator
		{
		public:
			StackAllocator(bool alignAlloc = true, size_t alignment = 16, size_t blockSize = 4096, size_t blockAllocCount=0);
			~StackAllocator()override;
			void* Allocate(size_t size, const char* fileName, const char* className, size_t line)override;
			void Deallocate(void*)override;
			bool IsAlignedAlloc()const { return m_alignAlloc; }
			const size_t GetAlignment()const { return m_alignment; }
			const size_t GetBlockSize()const { return m_blockSize; }
			const size_t GetNonEmptyBlockCount()const;
#ifdef ENABLE_MEMORY_LOG
			void LogBlockUsage()const;
#endif
		private:
			struct MemoryNode
			{
				MemoryInfo basicInfo;
				size_t stackIndex;
				size_t nodeIndex;
				bool used;
			};
			void ReallocStacks();
	#ifdef ENABLE_MEMORY_LOG
			void LogMemory(const char* const logName, const MemoryNode* pNode);
	#endif
			struct InternalStack
			{
				char* buffer;
				MemoryNode* nodes;
				size_t allocCount;
				size_t topPointer;
				size_t basePointer;
				size_t bufferEnd;
				size_t index;
			};
			InternalStack* CreateInternalStack();
			const bool m_alignAlloc;
			const size_t m_alignment;
			const size_t m_blockSize;
			const size_t m_internalMemoryNodeStep;
			InternalStack** m_stacks;
			size_t m_currentStack;
			size_t m_maxStack;
			size_t m_reallocStep;
			const size_t m_internalStackAllocStep = 100;
			const size_t m_reallocStackFactor = 2;
		};
	}

}
