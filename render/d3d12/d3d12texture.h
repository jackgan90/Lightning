#pragma once
#include "itexture.h"

namespace Lightning
{
	namespace Render
	{
		class D3D12Texture : public ITexture
		{
		public:
			void Commit()override;
		};
	}
}