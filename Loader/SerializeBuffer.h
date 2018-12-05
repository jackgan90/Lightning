#pragma once
#include "ISerializeBuffer.h"

namespace Lightning
{
	namespace Loading
	{
		class SerializeBuffer : public ISerializeBuffer
		{
		public:
			SerializeBuffer(std::size_t size);
			~SerializeBuffer()override;
			char* GetBuffer()override;
			std::size_t GetBufferSize()const override;
		private:
			char* mBuffer;
			std::size_t mSize;
		};
	}
}