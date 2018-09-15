#pragma once
#include "imemoryallocator.h"
//#define ENABLE_MEMORY_LOG

namespace Lightning
{
	namespace Foundation
	{
		//The memory layout of the memory store is like this:user mem0, memoryNode, user mem1, memoryNode, user mem2, memoryNode ...
		template<bool AlignedAlloc = true, const size_t Alignment = 16, const size_t BlockSize = 8192>
		class StackAllocator : public IMemoryAllocator
		{
		public:
			StackAllocator(size_t blockAllocCount=0);
			~StackAllocator()override;
			void* Allocate(size_t size, const char* fileName, const char* className, size_t line)override;
			void Deallocate(void*)override;
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
			const size_t m_internalMemoryNodeStep;
			InternalStack** m_stacks;
			size_t m_currentStack;
			size_t m_maxStack;
			size_t m_reallocStep;
			const size_t m_internalStackAllocStep = 100;
			const size_t m_reallocStackFactor = 2;
		};

	#ifdef ENABLE_MEMORY_LOG
		extern Logger logger;
	#endif
		template<bool AlignedAlloc, const size_t Alignment, const size_t BlockSize>
		StackAllocator<AlignedAlloc, Alignment, BlockSize>::StackAllocator(size_t blockAllocCount):IMemoryAllocator()
			,m_internalMemoryNodeStep(blockAllocCount > 0 ? std::min(BlockSize, blockAllocCount) : BlockSize)
		{
			//alignment should be a power of 2
			static_assert(Alignment > 0 && (Alignment & (Alignment - 1)) == 0, "Use of Non-power-of-2 alignment is forbidden.");
			m_stacks = new InternalStack*[m_internalStackAllocStep];
			m_maxStack = m_currentStack = 0;
			m_reallocStep = m_internalStackAllocStep;
			CreateInternalStack();
		}

		template<bool AlignedAlloc, const size_t Alignment, const size_t BlockSize>
		void StackAllocator<AlignedAlloc, Alignment, BlockSize>::ReallocStacks()
		{
			auto originalStackSize = m_reallocStep;
			m_reallocStep *= m_reallocStackFactor;
			auto newStacks = new InternalStack*[m_reallocStep];
			std::memcpy(newStacks, m_stacks, originalStackSize * sizeof(InternalStack*));
			delete[] m_stacks;
			m_stacks = newStacks;
		}

		template<bool AlignedAlloc, const size_t Alignment, const size_t BlockSize>
		typename StackAllocator<AlignedAlloc, Alignment, BlockSize>::InternalStack* StackAllocator<AlignedAlloc, Alignment, BlockSize>::CreateInternalStack()
		{
			//TODO resolve situation where there's not enough stack
			if (m_maxStack >= m_reallocStep)
			{
				ReallocStacks();
			}
			auto stack = new InternalStack;
			stack->buffer = new char[BlockSize];
			stack->bufferEnd = reinterpret_cast<size_t>(stack->buffer) + BlockSize;
			stack->allocCount = 0;
			stack->nodes = new MemoryNode[m_internalMemoryNodeStep];
			//according to c++ standard,the returned pointer of new always ensures enough space for a pointer decrement,so just
			//MakeAlign won't cause heap corruption
			if (AlignedAlloc)
				stack->basePointer = MakeAlign(reinterpret_cast<size_t>(stack->buffer), Alignment);
			else
				stack->basePointer = reinterpret_cast<size_t>(stack->buffer) + sizeof(MemoryNode**);
			stack->topPointer = stack->basePointer;
			stack->index = m_maxStack;
			m_currentStack = m_maxStack;
			m_stacks[m_maxStack++] = stack;
			return stack;
		}

		template<bool AlignedAlloc, const size_t Alignment, const size_t BlockSize>
		StackAllocator<AlignedAlloc, Alignment, BlockSize>::~StackAllocator()
		{
			for (unsigned int i = 0;i < m_maxStack;++i)
			{
				delete[] m_stacks[i]->buffer;
				delete[] m_stacks[i]->nodes;
				delete m_stacks[i];
			}
			delete[] m_stacks;
		}

		template<bool AlignedAlloc, const size_t Alignment, const size_t BlockSize>
		void* StackAllocator<AlignedAlloc, Alignment, BlockSize>::Allocate(size_t size, const char* fileName, const char* className, size_t line)
		{
			assert(size > 0);
			assert(size <= BlockSize - Alignment && "StackAllocator is not able to allocate memory which size is bigger than BlockSize - Alignment");
			unsigned int stackIndex = m_currentStack;
			auto stack = m_stacks[stackIndex];
			size_t anchor;
			if(!AlignedAlloc)
				anchor = stack->topPointer;
			else
			{
				if (stack->topPointer == stack->basePointer)
				{
					anchor = stack->topPointer;
				}
				else
				{
					anchor = MakeAlign(stack->topPointer, Alignment);
					while (anchor - stack->topPointer < sizeof(MemoryNode**))
						anchor = MakeAlign(anchor, Alignment);
				}
			}
			while (stackIndex < m_maxStack && (anchor + size >= stack->bufferEnd || stack->allocCount >= m_internalMemoryNodeStep))
			{
				++stackIndex;
				if (stackIndex >= m_maxStack)
					break;
				stack = m_stacks[stackIndex];
				if (AlignedAlloc)
				{
					anchor = stack->topPointer == stack->basePointer ? stack->topPointer : MakeAlign(stack->topPointer, Alignment);
				}
				else
				{
					anchor = stack->topPointer;
				}
			}
			if (stackIndex >= m_maxStack)
			{
				stack = CreateInternalStack();
				anchor = stack->topPointer;
			}
			else
			{
				stack = m_stacks[stackIndex];
				m_currentStack = stackIndex;
			}

			assert(anchor + size < stack->bufferEnd && "Can't allocate memory of size size due to low block size!You should specify a bigger block size!");
			auto& node = stack->nodes[stack->allocCount];
			*(reinterpret_cast<MemoryNode**>(anchor) - 1) = &node;
			
			void* mem = reinterpret_cast<void*>(anchor);
			node.basicInfo.address = mem;
			node.basicInfo.size = size;
#ifndef NDEBUG
			node.basicInfo.fileName = fileName;
			node.basicInfo.className = className;
			node.basicInfo.line = line;
#endif
			node.used = true;
			node.stackIndex = stack->index;
			node.nodeIndex = stack->allocCount;
			++stack->allocCount;
			stack->topPointer = anchor + size + sizeof(MemoryNode**);
			m_allocatedSize += size;
			++m_allocatedCount;
#ifdef ENABLE_MEMORY_LOG
			LogMemory("Allocate", &node);
#endif

			return mem;
		}


		template<bool AlignedAlloc, const size_t Alignment, const size_t BlockSize>
		void StackAllocator<AlignedAlloc, Alignment, BlockSize>::Deallocate(void* p)
		{
			auto node = *(reinterpret_cast<MemoryNode**>(p) - 1);
			node->used = false;
			auto stack = m_stacks[node->stackIndex];
			while (node->nodeIndex == stack->allocCount - 1 && !node->used)
			{
				--stack->allocCount;
				--m_allocatedCount;
				m_allocatedSize -= node->basicInfo.size;
				stack->topPointer = reinterpret_cast<size_t>(node->basicInfo.address);
#ifdef ENABLE_MEMORY_LOG
				LogMemory("Deallocate", node);
#endif
				if (node->nodeIndex > 0)
				{
					node = &stack->nodes[node->nodeIndex - 1];
				}
				else
				{
					assert(stack->topPointer == stack->basePointer);
					m_currentStack = stack->index;
					break;
				}
			}
		}

		template<bool AlignedAlloc, const size_t Alignment, const size_t BlockSize>
		const size_t StackAllocator<AlignedAlloc, Alignment, BlockSize>::GetNonEmptyBlockCount()const
		{
			size_t count{0};
			for (unsigned int i = 0; i < m_maxStack; ++i)
			{
				count += m_stacks[i]->topPointer == m_stacks[i]->basePointer ? 0 : 1;
			}
			return count;
		}

	#ifdef ENABLE_MEMORY_LOG
		void StackAllocator::LogMemory(const char* const logName, const MemoryNode* pNode)
		{
			std::string typeStr;
#ifndef NDEBUG
			const char* format = "%s memory at 0x%x, size = %d. file:%s, function : %s, line:%d.";
#else
			const char* format = "%s memory at 0x%x, size = %d.";
#endif
			logger.Log(LogLevel::Debug, format, logName, reinterpret_cast<size_t>(pNode->basicInfo.address), pNode->basicInfo.size
#ifndef NDEBUG
				, pNode->basicInfo.fileName, pNode->basicInfo.className, pNode->basicInfo.line);
#else
				);
#endif

		}

		void StackAllocator::LogBlockUsage()const
		{
			for (unsigned int i = 0;i < m_maxStack;++i)
			{
				size_t usedBytes = m_stacks[i]->topPointer - m_stacks[i]->basePointer;

				logger.Log(LogLevel::Debug, "Block 0x%x use %d bytes.", reinterpret_cast<size_t>(m_stacks[i]->buffer), usedBytes);
			}
			logger.Log(LogLevel::Debug, "Total block count:%d", m_maxStack);
		}
	#endif
	}

}
