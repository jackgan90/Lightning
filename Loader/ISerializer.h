#pragma once
#include "IFileSystem.h"
#include "ISerializeBuffer.h"


namespace Lightning
{
	namespace Loading
	{
		enum LoadType
		{
			LOAD_TYPE_TEXTURE = 0,
			LOAD_TYPE_SHADER,
			LOAD_TYPE_NUM
		};

		struct ISerializer
		{
			virtual ~ISerializer() = default;
			virtual std::shared_ptr<ISerializeBuffer> Serialize() = 0;
			virtual void Deserialize(Foundation::IFile* file, const std::shared_ptr<ISerializeBuffer>& buffer) = 0;
		};
	}
}