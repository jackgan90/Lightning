#pragma once
#include <algorithm>
#include <type_traits>
#include "foundationexportdef.h"
#include "imemoryallocator.h"

namespace LightningGE
{
	namespace Foundation
	{
		template<typename T, const size_t ObjectCount, bool AlignedAlloc, const size_t Alignment>
		class LIGHTNINGGE_FOUNDATION_API PoolAllocator : public IMemoryAllocator
		{
		public:	
			PoolAllocator();
			~PoolAllocator()override;
			void* Allocate(size_t size, const char* fileName, const char* className, size_t line)override;
			void Deallocate(void*)override;
			template<typename... Args>
			T* GetObject(Args&&... args);
			void ReleaseObject(T* pObj);
		private:
			struct PoolObject
			{
				PoolObject* next;
			};
			template<bool _Aligned>
			typename std::enable_if<_Aligned, char*>::type AllocInternalBuffer();
			template<bool _Aligned>
			typename std::enable_if<!_Aligned, char*>::type AllocInternalBuffer();
			template<bool _Aligned>
			typename std::enable_if<_Aligned>::type InitLinkList();
			template<bool _Aligned>
			typename std::enable_if<!_Aligned>::type InitLinkList();
			char* m_buffer;
			PoolObject* m_head;
		};

		template<typename T, const size_t ObjectCount, bool AlignedAlloc, const size_t Alignment>
		PoolAllocator<T, ObjectCount, AlignedAlloc, Alignment>::PoolAllocator():m_buffer(nullptr),m_head(nullptr)
		{
			m_buffer = AllocInternalBuffer<AlignedAlloc>();
			InitLinkList<AlignedAlloc>();
		}

		template<typename T, const size_t ObjectCount, bool AlignedAlloc, const size_t Alignment>
		PoolAllocator<T, ObjectCount, AlignedAlloc, Alignment>::~PoolAllocator()
		{
			delete[] m_buffer;
		}

		template<typename T, const size_t ObjectCount, bool AlignedAlloc, const size_t Alignment>
		template<bool _Aligned>
		typename std::enable_if<_Aligned, char*>::type
		PoolAllocator<T, ObjectCount, AlignedAlloc, Alignment>::AllocInternalBuffer()		//aligned alloc version
		{
			size_t objSize = std::max(sizeof(T), sizeof(PoolObject));
			return objSize % Alignment ? new char[(objSize + Alignment) * ObjectCount] : new char[objSize * ObjectCount]
		}

		template<typename T, const size_t ObjectCount, bool AlignedAlloc, const size_t Alignment>
		template<bool _Aligned>
		typename std::enable_if<!_Aligned, char*>::type
		PoolAllocator<T, ObjectCount, AlignedAlloc, Alignment>::AllocInternalBuffer() //normal version
		{
			return new char[std::max(sizeof(T), sizeof(PoolObject)) * ObjectCount];
		}

		template<typename T, const size_t ObjectCount, bool AlignedAlloc, const size_t Alignment>
		template<bool _Aligned>
		typename std::enable_if<_Aligned>::type
		PoolAllocator<T, ObjectCount, AlignedAlloc, Alignment>::InitLinkList() //aligned linklist version
		{
			size_t current = reinterpret_cast<size_t>(m_buffer);
			size_t objSize = std::max(sizeof(T), sizeof(PoolObject));
			for (size_t i = 0; i < ObjectCount; ++i)
			{
				current = current % Alignment ? MakeAlign(current, Alignment) : current;
				PoolObject* pObj = reinterpret_cast<PoolObject*>(current);
				if (!m_head)
					m_head = pObj;
				pObj->next = i == ObjectCount - 1 ? nullptr : reinterpret_cast<PoolObject*>(
					(current+objSize) % Alignment ? (MakeAlign(current+objSize, Alignment)) : (current + objSize));
				current += objSize;
			}
		}

		template<typename T, const size_t ObjectCount, bool AlignedAlloc, const size_t Alignment>
		template<bool _Aligned>
		typename std::enable_if<!_Aligned>::type
		PoolAllocator<T, ObjectCount, AlignedAlloc, Alignment>::InitLinkList() //normal linklist version
		{
			size_t current = reinterpret_cast<size_t>(m_buffer);
			size_t objSize = std::max(sizeof(T), sizeof(PoolObject));
			for (size_t i = 0; i < ObjectCount; ++i)
			{
				PoolObject* pObj = reinterpret_cast<PoolObject*>(current);
				if (!m_head)
					m_head = pObj;
				pObj->next = i == ObjectCount - 1 ? nullptr : reinterpret_cast<PoolObject*>(current + objSize);
				current += objSize;
			}
		}

		template<typename T, const size_t ObjectCount, bool AlignedAlloc, const size_t Alignment>
		void* PoolAllocator<T, ObjectCount, AlignedAlloc, Alignment>::Allocate(size_t size, const char* fileName, const char* className, size_t line)
		{
			if (!m_head)
				return nullptr;
			auto current = m_head;
			m_head = m_head->next;
			++m_allocatedCount;
			m_allocatedSize += sizeof(T);
			return current;
		}

		template<typename T, const size_t ObjectCount, bool AlignedAlloc, const size_t Alignment>
		void PoolAllocator<T, ObjectCount, AlignedAlloc, Alignment>::Deallocate(void* p)
		{
			--m_allocatedCount;
			m_allocatedSize -= sizeof(T);
			if (!m_head)
			{
				m_head = reinterpret_cast<PoolObject*>(p);
				m_head->next = nullptr;
			}
			else
			{
				PoolObject* tmp = reinterpret_cast<PoolObject*>(p);
				tmp->next = m_head;
				m_head = tmp;
			}
		}

		template<typename T, const size_t ObjectCount, bool AlignedAlloc, const size_t Alignment>
		template<typename... Args>
		T* PoolAllocator<T, ObjectCount, AlignedAlloc, Alignment>::GetObject(Args&&... args)
		{
			T* obj = Allocate(0, nullptr, nullptr, 0);
			if (obj)
				obj->T(std::forward<Args>(args)...);
			return obj;
		}

		template<typename T, const size_t ObjectCount, bool AlignedAlloc, const size_t Alignment>
		void PoolAllocator<T, ObjectCount, AlignedAlloc, Alignment>::ReleaseObject(T* p)
		{
			p->~T();
			Deallocate(p);
		}

	}
}