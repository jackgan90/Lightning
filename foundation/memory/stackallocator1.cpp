#include <cassert>
#include <cstdlib>
//For Debug purpose
#include <chrono>
#include <iostream>
//For Debug purpose
#include "stackallocator1.h"
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
			StackAllocator1::StackAllocator1(bool alignAlloc, size_t alignment, size_t blockSize):IMemoryAllocator()
				,m_blockSize(blockSize), m_alignment(alignment), m_alignAlloc(alignAlloc)
			{
				//alignment should be a power of 2
				assert(alignment > 0 && (alignment & (alignment - 1)) == 0);
				m_stacks = new InternalStack*[m_internalStackAllocStep];
				m_maxStack = m_currentStack = 0;
				CreateInternalStack();
			}

			StackAllocator1::InternalStack* StackAllocator1::CreateInternalStack()
			{
				//TODO resolve situation where there's not enough stack
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

			StackAllocator1::~StackAllocator1()
			{
				for (unsigned int i = 0;i < m_maxStack;++i)
				{
					delete[] m_stacks[i]->buffer;
					delete[] m_stacks[i]->nodes;
				}
				delete[] m_stacks;
			}

			void* StackAllocator1::Allocate(size_t size, const char* fileName, const char* className, size_t line)
			{
				unsigned int stackIndex = m_currentStack;
				auto stack = m_stacks[stackIndex];
				while (stackIndex < m_maxStack && stack->topPointer + size >= stack->bufferEnd)
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
				}

				auto node = stack->nodes[stack->allocCount];
				*(reinterpret_cast<MemoryNode**>(stack->topPointer) - 1) = &node;
				
				void* mem = reinterpret_cast<void*>(stack->topPointer);
				node.basicInfo.address = mem;
				node.basicInfo.size = size;
				node.used = true;
				node.stackIndex = stack->index;
				node.nodeIndex = stack->allocCount;
				++stack->allocCount;
				stack->topPointer += size + sizeof(size_t);

				return mem;
			}

//			void* StackAllocator1::AllocateInMemoryStore(void* pMemStore, void* pos, size_t size, 
//				const char* fileName, const char* className, size_t line)
//			{
//				auto top = m_memoryStore[pMemStore];
//				void* ret = pos;
//				if (!ret)	//only when pMemStore is a newly allocated store will pos be nullptr
//				{
//					if (m_alignAlloc)
//						ret = MakeAlign(pMemStore);
//					else
//						ret = pMemStore;
//				}
//				MemoryNode* node = new (reinterpret_cast<void*>(reinterpret_cast<size_t>(ret) + size)) MemoryNode;
//				node->basicInfo.address = ret;
//				node->basicInfo.size = size;
//#ifndef NDEBUG
//				node->basicInfo.className = className;
//				node->basicInfo.fileName = fileName;
//				node->basicInfo.line = line;
//#endif
//				node->memoryStorePtr = pMemStore;
//				node->prevNode = top;
//				node->used = true;
//				m_allocationMap[ret] = node;
//				m_memoryStore[pMemStore] = node;
//		#ifdef ENABLE_MEMORY_LOG
//				LogMemory("Allocate", node);
//		#endif
//				m_allocatedSize += size;
//				m_allocatedCount++;
//				return ret;
//			}


			void StackAllocator1::Deallocate(void* p)
			{

				auto node = *(reinterpret_cast<MemoryNode**>(p) - 1);
				node->used = false;
				auto stack = m_stacks[node->stackIndex];
				while (node->nodeIndex == stack->allocCount - 1 && !node->used)
				{
					--stack->allocCount;
					if (node->nodeIndex > 0)
						node = &stack->nodes[node->nodeIndex - 1];
					else
						break;
				}
			}

			inline void * StackAllocator1::MakeAlign(void * ptr)
			{
				return reinterpret_cast<void*>((reinterpret_cast<std::size_t>(ptr) & ~std::size_t(m_alignment - 1)) + m_alignment);
			}

			void* StackAllocator1::FindPosition(void* pMemStore, size_t size)
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
			void StackAllocator1::LogMemory(const char* const logName, const MemoryNode* pNode)
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

			void StackAllocator1::LogBlockUsage()const
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
