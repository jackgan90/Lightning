#pragma once
#include "idevice.h"

namespace LightningGE
{
	namespace Renderer
	{
		class LIGHTNINGGE_RENDERER_API D3D12Device : public IDevice
		{
		public:
			~D3D12Device()override;
		};
	}
}