#pragma once
#include <memory>
#include <unordered_map>
#include <tuple>
#include "rendererexportdef.h"
#include "irendertarget.h"
#include "hashableobject.h"

namespace LightningGE
{
	namespace Renderer
	{
		using Utility::HashableObject;
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

		class LIGHTNINGGE_RENDERER_API IBlendState : public HashableObject
		{
		public:
			virtual ~IBlendState() = default;
			virtual const BlendConfiguration* GetConfiguration(const RenderTargetPtr& renderTarget) = 0;
			virtual bool SetConfiguration(const RenderTargetPtr& renderTarget, const BlendConfiguration& configuration) = 0;
			virtual bool RemoveConfiguration(const RenderTargetPtr& renderTarget) = 0;
			virtual bool Enable(const RenderTargetPtr& renderTarget, bool enable) = 0;
			virtual bool SetFactors(const RenderTargetPtr& renderTarget, BlendFactor srcColor, BlendFactor srcAlpha, BlendFactor destColor, BlendFactor destAlpha) = 0;
		};
		typedef std::shared_ptr<IBlendState> BlendStatePtr;

		class LIGHTNINGGE_RENDERER_API BlendState : public IBlendState
		{
		public:
			BlendState();
			const BlendConfiguration* GetConfiguration(const RenderTargetPtr& renderTarget)override;
			bool SetConfiguration(const RenderTargetPtr& renderTarget, const BlendConfiguration& configuration)override;
			bool RemoveConfiguration(const RenderTargetPtr& renderTarget)override;
			bool Enable(const RenderTargetPtr& renderTarget, bool enable)override;
			bool SetFactors(const RenderTargetPtr& renderTarget, BlendFactor srcColor, BlendFactor srcAlpha, BlendFactor destColor, BlendFactor destAlpha)override;
		protected:
			size_t CalculateHashInternal()override;
			struct _PerTargetConfig
			{
				RenderTargetPtr renderTarget;
				BlendConfiguration config;
				int index;
			};
			typedef std::tuple<_PerTargetConfig*, std::unordered_map<RenderTargetID, _PerTargetConfig>::iterator> InternalConfigResult;
			InternalConfigResult InternalGetPerTargetConfig(const RenderTargetPtr& renderTarget);
			std::unordered_map<RenderTargetID, _PerTargetConfig> m_config;
			int m_lastIndex;
		};
	}
}