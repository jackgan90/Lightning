#include <cassert>
#include <cstdlib>
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
				//TODO add assertion
				MakeNewMemoryStore();
			}

			void* StackAllocator::MakeNewMemoryStore()
			{
				void* store = nullptr;
				if (m_alignAlloc)
				{
					store = std::malloc(m_blockSize + m_alignment);
					m_memoryStore.insert(std::make_pair(store, nullptr));
				}
				else
				{
					store = std::malloc(m_blockSize);
					m_memoryStore.insert(std::make_pair(store, nullptr));
				}
				return store;
			}


			StackAllocator::~StackAllocator()
			{
				for (auto it = m_memoryStore.begin();it != m_memoryStore.end();++it)
				{
					std::free(it->first);
				}
				m_memoryStore.clear();
				m_allocationMap.clear();
			}

			void* StackAllocator::Allocate(size_t size, const char* fileName, const char* className, size_t line)
			{
				assert(size <= m_blockSize - sizeof(MemoryNode));
				for (auto it = m_memoryStore.begin(); it != m_memoryStore.end(); ++it)
				{
					auto pos = FindPosition(it->first, size);
					if(pos)
						return AllocateInMemoryStore(it->first, pos, size, fileName, className, line);
				}
				auto newMemStore = MakeNewMemoryStore();
				return AllocateInMemoryStore(newMemStore, nullptr, size, fileName, className, line);
			}

			void* StackAllocator::AllocateInMemoryStore(void* pMemStore, void* pos, size_t size, 
				const char* fileName, const char* className, size_t line)
			{
				auto top = m_memoryStore[pMemStore];
				void* ret = pos;
				if (!ret)	//only when pMemStore is a newly allocated store will pos be nullptr
				{
					if (m_alignAlloc)
						ret = MakeAlign(pMemStore);
					else
						ret = pMemStore;
				}
				MemoryNode* node = new (reinterpret_cast<void*>(reinterpret_cast<size_t>(ret) + size)) MemoryNode;
				node->basicInfo.address = ret;
				node->basicInfo.size = size;
#ifndef NDEBUG
				node->basicInfo.className = className;
				node->basicInfo.fileName = fileName;
				node->basicInfo.line = line;
#endif
				node->memoryStorePtr = pMemStore;
				node->prevNode = top;
				node->used = true;
				m_allocationMap[ret] = node;
				m_memoryStore[pMemStore] = node;
		#ifdef ENABLE_MEMORY_LOG
				LogMemory("Allocate", node);
		#endif
				m_allocatedSize += size;
				m_allocatedCount++;
				return ret;
			}


			void StackAllocator::Deallocate(void* p)
			{
				auto itMemNode = m_allocationMap.find(p);
				assert(itMemNode != m_allocationMap.end());
				auto memoryNode = itMemNode->second;
				auto storePtr = memoryNode->memoryStorePtr;
				memoryNode->used = false;
				auto top = m_memoryStore[storePtr];
				if(memoryNode == top) //Only when we release the stack top element should we release other nodes
				{
					while (memoryNode && !memoryNode->used)
					{
		#ifdef ENABLE_MEMORY_LOG
						LogMemory("Deallocate", memoryNode);
		#endif
						m_allocatedSize -= memoryNode->basicInfo.size;
						m_allocatedCount--;
						m_allocationMap.erase(memoryNode->basicInfo.address);
						memoryNode = memoryNode->prevNode;
					}
					m_memoryStore[storePtr] = memoryNode;
				}
			}
			inline void * StackAllocator::MakeAlign(void * ptr)
			{
				return reinterpret_cast<void*>((reinterpret_cast<size_t>(ptr) & ~m_alignment) + m_alignment);
			}

			void* StackAllocator::FindPosition(void* pMemStore, size_t size)
			{
				auto top = m_memoryStore[pMemStore];
				void* anchor = top ? reinterpret_cast<void*>(top) : pMemStore;
				size_t anchorSize = top ? sizeof(MemoryNode) : 0;
				size_t start, end;
				if (m_alignAlloc)
				{
					start = reinterpret_cast<size_t>(MakeAlign(reinterpret_cast<void*>(reinterpret_cast<size_t>(anchor) + anchorSize)));
					end = reinterpret_cast<size_t>(pMemStore) + m_blockSize + m_alignment - sizeof(MemoryNode);
				}
				else
				{
					start = reinterpret_cast<size_t>(reinterpret_cast<void*>(reinterpret_cast<size_t>(anchor) + anchorSize));
					end = reinterpret_cast<size_t>(pMemStore) + m_blockSize - sizeof(MemoryNode);
				}
				//assert(start - reinterpret_cast<size_t>(pMemStore) < m_blockSize);
				return end > start && size + sizeof(MemoryNode) <= end - start ? reinterpret_cast<void*>(start) : nullptr;
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
