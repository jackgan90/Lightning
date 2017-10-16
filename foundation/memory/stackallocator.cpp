#include <cassert>
#include <cstdlib>
//For Debug purpose
#include <chrono>
#include <iostream>
//For Debug purpose
#include "stackallocator.h"
#ifdef ENABLE_MEMORY_LOG
#include "logger.h"
#endif

namespace LightningGE
{
	namespace Foundation
	{
		#ifdef ENABLE_MEMORY_LOG
			extern Logger logger;
		#endif
			StackAllocator::StackAllocator(bool alignAlloc, size_t alignment, size_t blockSize):IMemoryAllocator()
				,m_blockSize(blockSize), m_alignment(alignment), m_alignAlloc(alignAlloc)
			{
				//alignment should be a power of 2
				assert(alignment > 0 && (alignment & (alignment - 1)) == 0);
				m_stacks = new InternalStack*[m_internalStackAllocStep];
				m_maxStack = m_currentStack = 0;
				m_reallocStep = m_internalStackAllocStep;
				CreateInternalStack();
			}

			void StackAllocator::ReallocStacks()
			{
				auto originalStackSize = m_reallocStep;
				m_reallocStep *= m_reallocStackFactor;
				auto newStacks = new InternalStack*[m_reallocStep];
				std::memcpy(newStacks, m_stacks, originalStackSize * sizeof(InternalStack*));
				delete[] m_stacks;
				m_stacks = newStacks;
			}

			StackAllocator::InternalStack* StackAllocator::CreateInternalStack()
			{
				//TODO resolve situation where there's not enough stack
				if (m_maxStack >= m_reallocStep)
				{
					ReallocStacks();
				}
				auto stack = new InternalStack;
				stack->buffer = new char[m_blockSize];
				stack->bufferEnd = reinterpret_cast<size_t>(stack->buffer) + m_blockSize;
				stack->allocCount = 0;
				stack->nodes = new MemoryNode[m_internalMemoryNodeStep];
				stack->topPointer = reinterpret_cast<size_t>(stack->buffer) + sizeof(size_t);
				stack->index = m_maxStack;
				m_currentStack = m_maxStack;
				m_stacks[m_maxStack++] = stack;
				return stack;
			}

			StackAllocator::~StackAllocator()
			{
				for (unsigned int i = 0;i < m_maxStack;++i)
				{
					delete[] m_stacks[i]->buffer;
					delete[] m_stacks[i]->nodes;
				}
				delete[] m_stacks;
			}

			void* StackAllocator::Allocate(size_t size, const char* fileName, const char* className, size_t line)
			{
				unsigned int stackIndex = m_currentStack;
				auto stack = m_stacks[stackIndex];
				while (stackIndex < m_maxStack && (stack->topPointer + size >= stack->bufferEnd || stack->allocCount >= m_internalMemoryNodeStep))
				{
					++stackIndex;
					stack = m_stacks[stackIndex];
				}
				if (stackIndex >= m_maxStack)
				{
					stack = CreateInternalStack();
				}
				else
				{
					stack = m_stacks[stackIndex];
					m_currentStack = stackIndex;
				}

				auto& node = stack->nodes[stack->allocCount];
				*(reinterpret_cast<MemoryNode**>(stack->topPointer) - 1) = &node;
				
				void* mem = reinterpret_cast<void*>(stack->topPointer);
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
				stack->topPointer += size + sizeof(size_t);
				m_allocatedSize += size;
				++m_allocatedCount;

				return mem;
			}


			void StackAllocator::Deallocate(void* p)
			{
				auto node = *(reinterpret_cast<MemoryNode**>(p) - 1);
				node->used = false;
				auto stack = m_stacks[node->stackIndex];
				while (node->nodeIndex == stack->allocCount - 1 && !node->used)
				{
					--stack->allocCount;
					--m_allocatedCount;
					m_allocatedSize -= node->basicInfo.size;
					stack->topPointer -= node->basicInfo.size + sizeof(size_t);
					if (node->nodeIndex > 0)
					{
						node = &stack->nodes[node->nodeIndex - 1];
					}
					else
					{
						m_currentStack = stack->index;
						break;
					}
				}
			}

			const size_t StackAllocator::GetNonEmptyBlockCount()const
			{
				size_t count = 0;
				for (unsigned int i = 0; i < m_maxStack; ++i)
				{
					count += m_stacks[i]->topPointer == reinterpret_cast<size_t>(m_stacks[i]->buffer) + sizeof(size_t) ? 0 : 1;
				}
				return count;
			}


			inline void * StackAllocator::MakeAlign(void * ptr)
			{
				return reinterpret_cast<void*>((reinterpret_cast<std::size_t>(ptr) & ~std::size_t(m_alignment - 1)) + m_alignment);
			}

			void* StackAllocator::FindPosition(void* pMemStore, size_t size)
			{
				return nullptr;
				//auto top = m_memoryStore[pMemStore];
				//void* anchor = top ? reinterpret_cast<void*>(top) : pMemStore;
				//size_t anchorSize = top ? sizeof(MemoryNode) : 0;
				//size_t start, end;
				//if (m_alignAlloc)
				//{
				//	start = reinterpret_cast<size_t>(MakeAlign(reinterpret_cast<void*>(reinterpret_cast<size_t>(anchor) + anchorSize)));
				//	end = reinterpret_cast<size_t>(pMemStore) + m_blockSize + m_alignment - sizeof(MemoryNode);
				//}
				//else
				//{
				//	start = reinterpret_cast<size_t>(reinterpret_cast<void*>(reinterpret_cast<size_t>(anchor) + anchorSize));
				//	end = reinterpret_cast<size_t>(pMemStore) + m_blockSize - sizeof(MemoryNode);
				//}
				////assert(start - reinterpret_cast<size_t>(pMemStore) < m_blockSize);
				//return end > start && size + sizeof(MemoryNode) <= end - start ? reinterpret_cast<void*>(start) : nullptr;
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
				for (const auto it : m_memoryStore)
				{
					size_t usedBytes = 0;
					if (it.second)
					{
						usedBytes = reinterpret_cast<size_t>(it.second) + sizeof(MemoryNode) - reinterpret_cast<size_t>(it.first);
					}

					logger.Log(LogLevel::Debug, "Block %x use %d bytes.", reinterpret_cast<size_t>(it.first), usedBytes);
				}
			}
		#endif




	}
}
