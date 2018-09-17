#pragma once
#include "utilitiesexportdef.h"

namespace Lightning
{
	namespace Utility
	{
		class HashableObject
		{
		public:
			HashableObject() : mHashDirty(true), mHashValue(0){}
			virtual ~HashableObject() = default;
			size_t GetHashValue() { UpdateHash(); return mHashValue; }
		protected:
			virtual size_t CalculateHashInternal() = 0;
			void UpdateHash()
			{
				if (mHashDirty)
				{
					mHashValue = CalculateHashInternal();
					mHashDirty = false;
				}
			}
			void SetHashDirty()
			{
				mHashDirty = true;
			}
		private:
			bool mHashDirty;
			size_t mHashValue;
		};
	}
}