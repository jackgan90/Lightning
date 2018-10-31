#include <cassert>
#include "device.h"

namespace Lightning
{
	namespace Render
	{
		Device::Device()
		{

		}


		SharedShaderPtr Device::GetDefaultShader(ShaderType type)
		{
			auto it = mDefaultShaders.find(type);
			if (it == mDefaultShaders.end())
				return SharedShaderPtr();
			return it->second;
		}
	}
}