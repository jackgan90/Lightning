#include "MeshSerializer.h"

namespace Lightning
{
	namespace World
	{
		std::shared_ptr<ISerializeBuffer> MeshSerializer::Serialize()
		{
			return nullptr;
		}

		void MeshSerializer::Deserialize(Foundation::IFile* file, const std::shared_ptr<ISerializeBuffer>& buffer)
		{

		}
	}
}
