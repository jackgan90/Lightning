#pragma once
#include "ISerializer.h"

namespace Lightning
{
	namespace World
	{
		using Loading::ISerializeBuffer;
		class MeshSerializer : public Loading::ISerializer
		{
			std::shared_ptr<ISerializeBuffer> Serialize()override;
			void Deserialize(Foundation::IFile* file, const std::shared_ptr<ISerializeBuffer>& buffer)override;
		};
	}
}