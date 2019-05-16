#pragma once
#include <memory>
#include "ISerializer.h"

namespace Lightning
{
	namespace World
	{
		using Loading::ISerializer;
		struct ISerializable
		{
			virtual ~ISerializable() = default;
			//Creates a serializer which can be used to serialize this object
			virtual std::shared_ptr<ISerializer> CreateSerializer() = 0;
		};
	}
}