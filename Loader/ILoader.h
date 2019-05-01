#pragma once
#include <string>
#include <memory>
#include "ISerializer.h"

namespace Lightning
{
	namespace Loading
	{
		struct ILoader
		{
			virtual ~ILoader() = default;
			virtual void Finalize() = 0;
			virtual void Load(const std::string& path, const std::shared_ptr<ISerializer>& serializer) = 0;
		};
	}
}