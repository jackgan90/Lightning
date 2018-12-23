#pragma once
#include "ISerializeBuffer.h"
#include "RefObject.h"

namespace Lightning
{
	namespace Loading
	{
		class SerializeBuffer : public ISerializeBuffer
		{
		public:
			SerializeBuffer(std::size_t size);
			INTERFACECALL ~SerializeBuffer()override;
			char* INTERFACECALL GetBuffer()override;
			std::size_t INTERFACECALL GetBufferSize()const override;
		private:
			char* mBuffer;
			std::size_t mSize;
			REF_OBJECT_OVERRIDE(SerializeBuffer)
		};
	}
}