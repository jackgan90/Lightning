#pragma once
#include <memory>
#include <unordered_map>
#include "rendererexportdef.h"
#include "irenderresourcekeeper.h"
#include "iblendstate.h"
#include "idepthstencilstate.h"
#include "irasterizerstate.h"
#include "ishader.h"
#include "hashableobject.h"

namespace LightningGE
{
	namespace Renderer
	{
		using Utility::HashableObject;
		class LIGHTNINGGE_RENDERER_API IPipelineStateObject : public IRenderResourceKeeper, public HashableObject
		{
		public:
			virtual bool ApplyBlendState(const BlendStatePtr& blendState) = 0;
			virtual bool ApplyDepthStencilState(const DepthStencilStatePtr& dsState) = 0;
			virtual bool ApplyRasterizerState(const RasterizerStatePtr& rasterizerState) = 0;
			virtual bool ApplyShader(ShaderType shaderType, const ShaderPtr& shader) = 0;
			virtual BlendStatePtr GetBlendState()const = 0;
			virtual DepthStencilStatePtr GetDepthStencilState()const = 0;
			virtual RasterizerStatePtr GetRasterizerState()const = 0;
			virtual ShaderPtr GetShader(ShaderType type)const = 0;
		};
		using PipelineStateObjectPtr = std::shared_ptr<IPipelineStateObject>;

		typedef std::unordered_map<ShaderType, ShaderPtr> PipelineShaderMap;
		class LIGHTNINGGE_RENDERER_API PipelineStateObject : public IPipelineStateObject
		{
		public:
			bool ApplyBlendState(const BlendStatePtr& blendState)override;
			bool ApplyDepthStencilState(const DepthStencilStatePtr& dsState)override;
			bool ApplyRasterizerState(const RasterizerStatePtr& rasterizerState)override;
			bool ApplyShader(ShaderType shaderType, const ShaderPtr& shader)override;
			BlendStatePtr GetBlendState()const override;
			DepthStencilStatePtr GetDepthStencilState()const override;
			RasterizerStatePtr GetRasterizerState()const override;
			ShaderPtr GetShader(ShaderType type)const override;
		protected:
			size_t CalculateHashInternal()override;
			size_t GetInitialHashSeed();
			BlendStatePtr m_blendState;
			DepthStencilStatePtr m_depthStencilState;
			RasterizerStatePtr m_rasterizerState;
			PipelineShaderMap m_shaders;
		};
	}
}