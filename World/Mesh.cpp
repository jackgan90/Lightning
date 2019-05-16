#include "Mesh.h"
#include "Serializers/MeshSerializer.h"

namespace Lightning
{
	namespace World
	{
		std::shared_ptr<ISerializer> Mesh::CreateSerializer()
		{
			return std::make_shared<MeshSerializer>();
		}
	}
}