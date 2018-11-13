#pragma once
#include "LoaderExportDef.h"
#include "filesystem.h"


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

		class LIGHTNING_LOADER_API ISerializer
		{
		public:
			virtual ~ISerializer(){}
			virtual void Serialize(char** buffer) = 0;
			virtual void Deserialize(const Foundation::SharedFilePtr& file, char* buffer) = 0;
			//The Dispose method is the last method called by Loader.Subclass of it should do clean up work(recyle memory)
			virtual void Dispose() = 0;
		};
	}
}