#pragma once
#include "ITexture.h"

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