#pragma once
#include "utilitiesexportdef.h"

namespace LightningGE
{
	namespace Utility
	{
		class HashableObject
		{
		public:
			explicit HashableObject() : m_hashDirty(true), m_hashValue(0){}
			virtual ~HashableObject() = default;
			size_t GetHashValue() { UpdateHash(); return m_hashValue; }
		protected:
			virtual size_t CalculateHashInternal() = 0;
			void UpdateHash()
			{
				if (m_hashDirty)
				{
					m_hashValue = CalculateHashInternal();
					m_hashDirty = false;
				}
			}
			void SetHashDirty()
			{
				m_hashDirty = true;
			}
		private:
			bool m_hashDirty;
			size_t m_hashValue;
		};
	}
}