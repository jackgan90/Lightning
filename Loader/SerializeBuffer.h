#pragma once
#include <memory>
#include "ISerializeBuffer.h"

namespace Lightning
{
	namespace Loading
	{
		class SerializeBuffer : public ISerializeBuffer
		{
		public:
			SerializeBuffer(std::size_t size);
			char* GetBuffer()override;
			std::size_t GetBufferSize()const override;
		private:
			std::unique_ptr<char[]> mBuffer;
			std::size_t mSize;
		};
	}
}