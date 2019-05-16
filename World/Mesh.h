#pragma once
#include "ISerializable.h"

namespace Lightning
{
	namespace World
	{
		class Mesh : public ISerializable
		{
		public:
			std::shared_ptr<ISerializer> CreateSerializer()override;
		};
	}
}