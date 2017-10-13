#pragma once
#include <memory>
#include <unordered_map>
#include "rendererexportdef.h"
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
		class LIGHTNINGGE_RENDERER_API IPipelineStateObject : public HashableObject
		{
		public:
			virtual bool ApplyBlendState(const SharedBlendStatePtr& blendState) = 0;
			virtual bool ApplyDepthStencilState(const SharedDepthStencilStatePtr& dsState) = 0;
			virtual bool ApplyRasterizerState(const SharedRasterizerStatePtr& rasterizerState) = 0;
			virtual bool ApplyShader(ShaderType shaderType, const SharedShaderPtr& shader) = 0;
			virtual SharedBlendStatePtr GetBlendState()const = 0;
			virtual SharedDepthStencilStatePtr GetDepthStencilState()const = 0;
			virtual SharedRasterizerStatePtr GetRasterizerState()const = 0;
			virtual SharedShaderPtr GetShader(ShaderType type)const = 0;
		};
		typedef std::shared_ptr<IPipelineStateObject> SharedPipelineStateObjectPtr;

		typedef std::unordered_map<ShaderType, SharedShaderPtr> PipelineShaderMap;
		class LIGHTNINGGE_RENDERER_API PipelineStateObject : public IPipelineStateObject
		{
		public:
			bool ApplyBlendState(const SharedBlendStatePtr& blendState)override;
			bool ApplyDepthStencilState(const SharedDepthStencilStatePtr& dsState)override;
			bool ApplyRasterizerState(const SharedRasterizerStatePtr& rasterizerState)override;
			bool ApplyShader(ShaderType shaderType, const SharedShaderPtr& shader)override;
			SharedBlendStatePtr GetBlendState()const override;
			SharedDepthStencilStatePtr GetDepthStencilState()const override;
			SharedRasterizerStatePtr GetRasterizerState()const override;
			SharedShaderPtr GetShader(ShaderType type)const override;
		protected:
			size_t CalculateHashInternal()override;
			size_t GetInitialHashSeed();
			SharedBlendStatePtr m_blendState;
			SharedDepthStencilStatePtr m_depthStencilState;
			SharedRasterizerStatePtr m_rasterizerState;
			PipelineShaderMap m_shaders;
		};
	}
}