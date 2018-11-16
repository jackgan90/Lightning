#pragma once
#include <cassert>
#include "RefCount.h"

namespace Lightning
{
	namespace Plugins
	{
		//A class similar to ComPtr,served as a cross-platform smart pointer for RefCount objects.
		//Useful for managing objects created by plugins
		template<typename T>
		class SmartPointer
		{
			static_assert(std::is_base_of<RefCount, T>::value, "T must be a subclass of RefCount!");
		public:
			explicit SmartPointer(T* object):pObject(object)
			{
				assert((pObject ? pObject->GetRefCount() == 1 : true) &&
					"A RefCount object managed by SmartPointer should have reference count of 1.");
			}
			explicit SmartPointer(nullptr_t) :pObject(nullptr)
			{

			}
			~SmartPointer()
			{
				if (pObject)
					pObject->Release();
			}

			SmartPointer(const SmartPointer<T>& sp)noexcept
			{
				if (sp.pObject)
					sp.pObject->AddRef();
				pObject = sp.pObject;
			}

			SmartPointer(SmartPointer<T>&& sp)noexcept
			{
				pObject = sp.pObject;
				sp.pObject = nullptr;
			}

			SmartPointer<T>& operator=(const SmartPointer<T>& sp)noexcept
			{
				if (this == &sp)
					return *this;
				if (pObject)
					pObject->Release();
				if (sp.pObject)
					sp.pObject->AddRef();
				return *this;
			}

			SmartPointer<T>& operator=(SmartPointer<T>&& sp)noexcept
			{
				if (this == &sp)
					return *this;
				if (pObject)
					pObject->Release();
				pObject = sp.pObject;
				sp.pObject = nullptr;
				return *this;
			}

			T& operator*()
			{
				return *pObject;
			}

			const T& operator*()const
			{
				return *pObject;
			}

			T* operator->()
			{
				return pObject;
			}

			const T* operator->()const
			{
				return pObject;
			}

			T* Get()const { return pObject; }

			operator T*() { return pObject; }
		private:
			T* pObject;
		};

		template<typename T, typename... Args>
		SmartPointer<T> MakeSmartPointer(Args&&... args)
		{
			return SmartPointer<T>(new T(std::forward<Args>(args)...));
		}
	}
}