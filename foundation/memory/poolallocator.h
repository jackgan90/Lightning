#pragma once
#include <algorithm>
#include <type_traits>
#include "imemoryallocator.h"

namespace LightningGE
{
	namespace Foundation
	{
		template<typename T, const size_t ObjectCount, bool AlignedAlloc, const size_t Alignment>
		class PoolAllocator : public IMemoryAllocator
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
			void InitLinkList();
			template<typename _T>
			typename std::enable_if<std::is_class<_T>::value>::type InvokeDestructor(_T* obj);
			template<typename _T>
			typename std::enable_if<!std::is_class<_T>::value>::type InvokeDestructor(_T* obj);
			char* m_buffer;
			PoolObject* m_head;
		};

		template<typename T, const size_t ObjectCount, bool AlignedAlloc, const size_t Alignment>
		PoolAllocator<T, ObjectCount, AlignedAlloc, Alignment>::PoolAllocator():m_buffer(nullptr),m_head(nullptr)
		{
			static_assert(!AlignedAlloc || Alignment > 0, "Alignment must be greater than 0 when AlignedAlloc");
			if (AlignedAlloc)
			{
				size_t objSize = std::max(sizeof(T), sizeof(PoolObject));
				m_buffer = objSize % Alignment ? new char[(objSize + Alignment) * ObjectCount] : new char[objSize * ObjectCount + Alignment];
				//m_buffer = new char[(objSize + Alignment) * ObjectCount];
			}
			else
			{
				m_buffer = new char[std::max(sizeof(T), sizeof(PoolObject)) * ObjectCount];
			}
			InitLinkList();
		}

		template<typename T, const size_t ObjectCount, bool AlignedAlloc, const size_t Alignment>
		PoolAllocator<T, ObjectCount, AlignedAlloc, Alignment>::~PoolAllocator()
		{
			delete[] m_buffer;
		}

		template<typename T, const size_t ObjectCount, bool AlignedAlloc, const size_t Alignment>
		void PoolAllocator<T, ObjectCount, AlignedAlloc, Alignment>::InitLinkList() //aligned linklist version
		{
			static_assert(!AlignedAlloc || Alignment > 0, "Alignment must be greater than 0.");
			size_t current = reinterpret_cast<size_t>(m_buffer);
			size_t objSize = std::max(sizeof(T), sizeof(PoolObject));
			for (size_t i = 0; i < ObjectCount; ++i)
			{
				size_t nextAddr;
				if(AlignedAlloc)
				{
					current = current % Alignment ? MakeAlign(current, Alignment) : current;
					nextAddr = current + objSize;
					nextAddr = nextAddr % Alignment ? MakeAlign(nextAddr, Alignment) : nextAddr;
				}
				else
				{
					nextAddr = current + objSize;
				}
				PoolObject* pObj = reinterpret_cast<PoolObject*>(current);
				if (!m_head)
					m_head = pObj;
				pObj->next = i == ObjectCount - 1 ? nullptr : reinterpret_cast<PoolObject*>(nextAddr);
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
			T* obj = reinterpret_cast<T*>(Allocate(0, nullptr, nullptr, 0));
			if (obj)
				new (obj) T(std::forward<Args>(args)...);
			return obj;
		}

		template<typename T, const size_t ObjectCount, bool AlignedAlloc, const size_t Alignment>
		void PoolAllocator<T, ObjectCount, AlignedAlloc, Alignment>::ReleaseObject(T* p)
		{
			InvokeDestructor<T>(p);
			Deallocate(p);
		}

		template<typename T, const size_t ObjectCount, bool AlignedAlloc, const size_t Alignment>
		template<typename _T>
		typename std::enable_if<std::is_class<_T>::value>::type 
		PoolAllocator<T, ObjectCount, AlignedAlloc, Alignment>::InvokeDestructor(_T* obj)
		{
			obj->~T();
		}

		template<typename T, const size_t ObjectCount, bool AlignedAlloc, const size_t Alignment>
		template<typename _T>
		typename std::enable_if<!std::is_class<_T>::value>::type 
		PoolAllocator<T, ObjectCount, AlignedAlloc, Alignment>::InvokeDestructor(_T* obj)
		{

		}

	}
}