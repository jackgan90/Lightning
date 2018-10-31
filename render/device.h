#pragma once
#include <memory>
#include "idevice.h"

namespace Lightning
{
	namespace Render
	{
		class LIGHTNING_RENDER_API Device : public IDevice
		{
		public:
			friend class Renderer;
			Device();
			SharedShaderPtr GetDefaultShader(ShaderType type)override;
		protected:
			using DefaultShaderMap = container::unordered_map<ShaderType, SharedShaderPtr>;
			DefaultShaderMap mDefaultShaders;
		};
	}
}
