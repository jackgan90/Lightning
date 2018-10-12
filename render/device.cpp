#include <cassert>
#include "device.h"

namespace Lightning
{
	namespace Render
	{
		Device::Device() :mFrameResourceIndex(0)
		{

		}


		SharedShaderPtr Device::GetDefaultShader(ShaderType type)
		{
			auto it = mDefaultShaders.find(type);
			if (it == mDefaultShaders.end())
				return SharedShaderPtr();
			return it->second;
		}

		void Device::BeginFrame(const std::size_t frameResourceIndex)
		{
			mFrameResourceIndex = frameResourceIndex;
		}
	}
}