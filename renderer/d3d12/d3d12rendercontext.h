#pragma once
#include "irendercontext.h"

namespace LightningGE
{
	namespace Renderer
	{
		using WindowSystem::WindowPtr;
		class LIGHTNINGGE_RENDERER_API D3D12RenderContext : public IRenderContext
		{
		public:
			~D3D12RenderContext()override;
			bool Init(WindowPtr pWindow)override;
		};
	}
}