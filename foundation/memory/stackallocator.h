#pragma once
#include "imemoryallocator.h"
#define ENABLE_MEMORY_LOG

namespace LightningGE
{
	namespace Foundation
	{
		//The memory layout of the memory store is like this:user mem0, memoryNode, user mem1, memoryNode, user mem2, memoryNode ...
		class LIGHTNINGGE_FOUNDATION_API StackAllocator : public IMemoryAllocator
		{
		public:
			StackAllocator(bool alignAlloc = true, size_t alignment = 16, size_t blockSize = 4096);
			~StackAllocator()override;
			void* Allocate(size_t size, const char* fileName, const char* className, size_t line)override;
			void Deallocate(void*)override;
			size_t GetNonEmptyBlockCount()const;
			bool IsAlignedAlloc()const { return m_alignAlloc; }
			const size_t GetAlignment()const { return m_alignment; }
			const size_t GetBlockSize()const { return m_blockSize; }
#ifdef ENABLE_MEMORY_LOG
			void LogBlockUsage()const;
#endif
		private:
			struct MemoryNode
			{
				MemoryInfo basicInfo;
				void* memoryStorePtr;
				bool used;
				MemoryNode* prevNode;
			};
			//align address pointed to by ptr to next alignment byte
			inline void* MakeAlign(void* ptr);
			//Find a suitable position in pMemStore to allocate size bytes
			void* FindPosition(void* pMemStore, size_t size);
	#ifdef ENABLE_MEMORY_LOG
			void LogMemory(const char* const logName, const MemoryNode* pNode);
	#endif
			void* MakeNewMemoryStore();
			void* AllocateInMemoryStore(void* pMemStore, void* pos, size_t size, const char* fileName, const char* className, size_t line);
			//key is memory address returned to caller, value encodes allocated memory info
			typedef std::unordered_map<void*, MemoryNode*> AllocationMap;
			//key is memory store(one block) start address, value is stack top MemoryNode pointer
			typedef std::unordered_map<void*, MemoryNode*> MemoryStoreMap;
			const bool m_alignAlloc;
			const size_t m_alignment;
			const size_t m_blockSize;
			AllocationMap m_allocationMap;
			MemoryStoreMap m_memoryStore;
		};
	}

}