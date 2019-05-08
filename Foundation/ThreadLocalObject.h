#pragma once
#include <functional>
#include "tbb/enumerable_thread_specific.h"

namespace Lightning
{
	namespace Foundation
	{
		template<typename T>
		class ThreadLocalObject
		{
		public:
			using type = typename tbb::enumerable_thread_specific<T>;
			using iterator = typename tbb::enumerable_thread_specific<T>::iterator;
			using const_iterator = typename tbb::enumerable_thread_specific<T>::const_iterator;
			using size_type = typename tbb::enumerable_thread_specific<T>::size_type;
			T* operator->()
			{
				return &mInstances.local();
			}
			const T* operator->()const
			{
				return &mInstances.local();
			}
			T& Local()
			{
				return mInstances.local();
			}
			const T& Local()const
			{
				return mInstances.local();
			}

			iterator begin()
			{
				return mInstances.begin();
			}
			const_iterator cbegin()const
			{
				return mInstances.begin();
			}
			iterator end()
			{
				return mInstances.end();
			}
			const_iterator cend()const
			{
				return mInstances.end();
			}
			size_type size()const
			{
				return mInstances.size();
			}
			void for_each(std::function<void(T&)> f)
			{
				for (auto it = begin(); it != end(); ++it)
				{
					f(*it);
				}
			}
			void for_each(std::function<void(const T&)> f)const
			{
				for (auto it = cbegin(); it != cend(); ++it)
				{
					f(*it);
				}
			}
		private:
			type mInstances;
		};
	}
}