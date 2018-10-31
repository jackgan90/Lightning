#pragma once
#include <functional>
#include "loaderexportdef.h"


namespace Lightning
{
	namespace Loader
	{
		enum LoaderType
		{
			LOADER_TYPE_TEXTURE = 0,
			LOADER_TYPE_SHADER,
			LOADER_TYPE_NUM
		};

		using LoadFinishHandler = std::function<void(void*)>;
		class LIGHTNING_LOADER_API ILoader
		{
		public:
			virtual ~ILoader(){}
			virtual void Load(const std::string& path, LoadFinishHandler callback) = 0;
		};
	}
}