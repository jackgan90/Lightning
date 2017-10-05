#pragma once
#include <memory>
#include "rendererexportdef.h"
#include "irendertarget.h"

namespace LightningGE
{
	namespace Renderer
	{
		enum BlendOperation
		{
			BLEND_ADD,
			BLEND_SUBTRACT,
			BLEND_REVERSE_SUBTRACT,
			BLEND_MIN,
			BLEND_MAX
		};

		enum BlendFactor
		{
			BLEND_ZERO,
			BLEND_ONE,
			BLEND_SRC_COLOR,	
			BLEND_INV_SRC_COLOR,
			BLEND_SRC_ALPHA,
			BLEND_INV_SRC_ALPHA,
			BLEND_DEST_COLOR,
			BLEND_INV_DEST_COLOR,
			BLEND_DEST_ALPHA,
			BLEND_INV_DEST_ALPHA,
			//TODO there's more advanced blend factor in recent graphics API ,need to add them in case of use
		};
		
		struct BlendConfiguration
		{
			BlendConfiguration():enable(false), colorOp(BLEND_ADD) ,alphaOp(BLEND_ADD)
				, srcColorFactor(BLEND_SRC_ALPHA) ,srcAlphaFactor(BLEND_SRC_ALPHA)
				, destColorFactor(BLEND_INV_SRC_ALPHA), destAlphaFactor(BLEND_INV_SRC_ALPHA)
			{
			}
			bool enable;
			BlendOperation colorOp;
			BlendOperation alphaOp;
			BlendFactor srcColorFactor;
			BlendFactor srcAlphaFactor;
			BlendFactor destColorFactor;
			BlendFactor destAlphaFactor;
		};

		class LIGHTNINGGE_RENDERER_API IBlendState
		{
		public:
			virtual ~IBlendState() {}
			virtual const BlendConfiguration* GetConfiguration(const RenderTargetPtr& renderTarget) = 0;
			virtual bool SetConfiguration(const RenderTargetPtr& renderTarget, const BlendConfiguration& configuration) = 0;
			virtual bool RemoveConfiguration(const RenderTargetPtr& renderTarget) = 0;
			virtual bool Enable(const RenderTargetPtr& renderTarget, bool enable) = 0;
			virtual bool SetFactors(const RenderTargetPtr& renderTarget, BlendFactor srcColor, BlendFactor srcAlpha, BlendFactor destColor, BlendFactor destAlpha) = 0;
		};
		using BlendStatePtr = std::shared_ptr<IBlendState>;
	}
}