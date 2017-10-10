#pragma once
#include <cassert>

namespace LightningGE
{
	namespace Foundation
	{
		template<typename T, const int MaxCount>
		class Counter
		{
		public:
			explicit Counter() 
			{
				assert(count < MaxCount);
				++count; 
			}
			virtual ~Counter() 
			{ 
				--count; 
			}
			Counter(const Counter<T, MaxCount>&) = delete;
			Counter<T, MaxCount>& operator=(const Counter<T, MaxCount>&) = delete;
		private:
			static int count;
		};

		template<typename T, const int MaxCount>
		int Counter<T, MaxCount>::count = 0;
	}
}