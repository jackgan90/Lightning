#pragma once
#include "loaderexportdef.h"
#include "filesystem.h"


namespace Lightning
{
	namespace Loader
	{
		enum LoadType
		{
			LOAD_TYPE_TEXTURE = 0,
			LOAD_TYPE_SHADER,
			LOAD_TYPE_NUM
		};

		class LIGHTNING_LOADER_API IDeserializer
		{
		public:
			virtual ~IDeserializer(){}
			virtual void Deserialize(const Foundation::SharedFilePtr& file, char* buffer, void** resource) = 0;
		};
	}
}