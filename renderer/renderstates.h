#pragma once
#include <functional>
#include <unordered_map>
#include <cstdint>
#include <boost/functional/hash.hpp>
#include "ishader.h"
#include "irendertarget.h"
#include "ivertexbuffer.h"
#include "types/rect.h"

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
		
		struct BlendState
		{
			BlendState():enable(false), colorOp(BLEND_ADD) ,alphaOp(BLEND_ADD)
				, srcColorFactor(BLEND_SRC_ALPHA) ,srcAlphaFactor(BLEND_SRC_ALPHA)
				, destColorFactor(BLEND_INV_SRC_ALPHA), destAlphaFactor(BLEND_INV_SRC_ALPHA)
				, renderTarget(nullptr)
			{
			}
			bool enable;
			BlendOperation colorOp;
			BlendOperation alphaOp;
			BlendFactor srcColorFactor;
			BlendFactor srcAlphaFactor;
			BlendFactor destColorFactor;
			BlendFactor destAlphaFactor;
			IRenderTarget* renderTarget;

			bool operator==(const BlendState& state)const noexcept
			{
				if (colorOp != state.colorOp)
				{
					return false;
				}

				if (alphaOp != state.alphaOp)
				{
					return false;
				}

				if (srcColorFactor != state.srcColorFactor)
				{
					return false;
				}

				if (srcAlphaFactor != state.srcAlphaFactor)
				{
					return false;
				}

				if (destColorFactor != state.destColorFactor)
				{
					return false;
				}

				if (destAlphaFactor != state.destAlphaFactor)
				{
					return false;
				}
				
				if (renderTarget != state.renderTarget)
				{
					return false;
				}

				return true;
			}

			bool operator!=(const BlendState& state)const noexcept
			{
				return !(*this == state);
			}
		};

		enum FillMode
		{
			FILLMODE_WIREFRAME,
			FILLMODE_SOLID
		};

		enum CullMode
		{
			CULLMODE_NONE,
			CULLMODE_FRONT,
			CULLMODE_BACK
		};

		enum FrontFaceWindingOrder
		{
			COUNTER_CLOCKWISE,
			CLOCKWISE
		};

		struct RasterizerState
		{
			RasterizerState() :fillMode(FILLMODE_SOLID), cullMode(CULLMODE_BACK), frontFaceWindingOrder(COUNTER_CLOCKWISE)
			{

			}
			FillMode fillMode;
			CullMode cullMode;
			FrontFaceWindingOrder frontFaceWindingOrder;

			bool operator==(const RasterizerState& state)const noexcept
			{
				if (fillMode != state.fillMode)
				{
					return false;
				}

				if (cullMode != state.cullMode)
				{
					return false;
				}

				if (frontFaceWindingOrder != state.frontFaceWindingOrder)
				{
					return false;
				}

				return true;
			}

			bool operator!=(const RasterizerState& state)const noexcept
			{
				return !(*this == state);
			}
		};

		enum CmpFunc
		{
			NEVER,
			LESS,
			EQUAL,
			LESS_EQUAL,
			GREATER,
			NOT_EQUAL,
			GREATER_EQUAL,
			ALWAYS
		};

		enum StencilOp
		{
			KEEP,
			ZERO,
			REPLACE,
			INCREASE_CLAMP,
			DECREASE_CLAMP,
			INVERT,
			INCREASE_WRAP,
			DECREASE_WRAP
		};

		struct StencilFace
		{
			StencilFace() : cmpFunc(ALWAYS), passOp(KEEP), failOp(KEEP), depthFailOp(KEEP)
			{
			}
			CmpFunc cmpFunc;
			StencilOp passOp;
			StencilOp failOp;
			StencilOp depthFailOp;

			bool operator==(const StencilFace& face)const noexcept
			{
				if (cmpFunc != face.cmpFunc)
				{
					return false;
				}

				if (passOp != face.passOp)
				{
					return false;
				}

				if (failOp != face.failOp)
				{
					return false;
				}

				if (depthFailOp != face.depthFailOp)
				{
					return false;
				}

				return true;
			}

			bool operator!=(const StencilFace& face)const noexcept
			{
				return !(*this == face);
			}
		};

		struct DepthStencilState
		{
			DepthStencilState() : depthTestEnable(true), depthWriteEnable(true), depthCmpFunc(LESS)
				,stencilEnable(false), stencilRef(0), stencilReadMask(0xff), stencilWriteMask(0xff)
				,frontFace(), backFace()
			{

			}
			//depth config
			bool depthTestEnable;
			bool depthWriteEnable;
			CmpFunc depthCmpFunc;
			//stencil config
			bool stencilEnable;
			unsigned char stencilRef;
			unsigned char stencilReadMask;
			unsigned char stencilWriteMask;
			StencilFace frontFace;
			StencilFace backFace;

			bool operator==(const DepthStencilState& state)const noexcept
			{
				if (depthTestEnable != state.depthTestEnable)
				{
					return false;
				}

				if (depthWriteEnable != state.depthWriteEnable)
				{
					return false;
				}

				if (depthCmpFunc != state.depthCmpFunc)
				{
					return false;
				}

				if (stencilEnable != state.stencilEnable)
				{
					return false;
				}

				if (stencilRef != state.stencilRef)
				{
					return false;
				}

				if (stencilReadMask != state.stencilReadMask)
				{
					return false;
				}

				if (stencilWriteMask != state.stencilWriteMask)
				{
					return false;
				}

				if (frontFace != state.frontFace)
				{
					return false;
				}

				if (backFace != state.backFace)
				{
					return false;
				}

				return true;
			}

			bool operator!=(const DepthStencilState& state)const noexcept
			{
				return !(*this == state);
			}
		};

		using VertexComponentBoundMap = std::unordered_map<VertexComponent, std::uint16_t>;
		struct PipelineState
		{
			RasterizerState rasterizerState;
			BlendState blendState;
			DepthStencilState depthStencilState;
			IShader* vs;
			IShader* fs;
			IShader* gs;
			IShader* hs;
			IShader* ds;
			VertexComponentBoundMap vertexComponents;

			bool operator==(const PipelineState& state)const noexcept
			{
				if (rasterizerState != state.rasterizerState)
				{
					return false;
				}

				if (blendState != state.blendState)
				{
					return false;
				}

				if (depthStencilState != state.depthStencilState)
				{
					return false;
				}
				
				if (vs != state.vs)
				{
					return false;
				}

				if (fs != state.fs)
				{
					return false;
				}

				if (gs != state.gs)
				{
					return false;
				}

				if (hs != state.hs)
				{
					return false;
				}

				if (ds != state.ds)
				{
					return false;
				}

				if (vertexComponents.size() != state.vertexComponents.size())
				{
					return false;
				}

				for (auto it=vertexComponents.cbegin();it != vertexComponents.cend();++it)
				{
					auto slot = state.vertexComponents.find(it->first);
					if (slot == state.vertexComponents.end())
					{
						return false;
					}
					if (it->second != slot->second)
					{
						return false;
					}
				}

				return true;
			}

			bool operator!=(const PipelineState& state)const noexcept
			{
				return !(*this == state);
			}
		};

		struct Viewport
		{
			EIGEN_MAKE_ALIGNED_OPERATOR_NEW
			RectF viewport;
		};

		struct ScissorRect
		{
			EIGEN_MAKE_ALIGNED_OPERATOR_NEW
			RectF scissorRect;
		};
	}
}

namespace std
{
	template<> struct hash<LightningGE::Renderer::RasterizerState>
	{
		std::size_t operator()(const LightningGE::Renderer::RasterizerState& state)const noexcept
		{
			std::size_t seed = 0;
			boost::hash_combine(seed, state.cullMode);
			boost::hash_combine(seed, state.fillMode);
			boost::hash_combine(seed, state.frontFaceWindingOrder);
			return seed;
		}
	};

	template<> struct hash<LightningGE::Renderer::BlendState>
	{
		std::size_t operator()(const LightningGE::Renderer::BlendState& state)const noexcept
		{
			std::size_t seed = 0;
			boost::hash_combine(seed, state.alphaOp);
			boost::hash_combine(seed, state.colorOp);
			boost::hash_combine(seed, state.destAlphaFactor);
			boost::hash_combine(seed, state.destColorFactor);
			boost::hash_combine(seed, state.enable);
			boost::hash_combine(seed, state.renderTarget? state.renderTarget->GetID() : 0);
			boost::hash_combine(seed, state.srcAlphaFactor);
			boost::hash_combine(seed, state.srcColorFactor);
			return seed;
		}
	};

	template<> struct hash<LightningGE::Renderer::DepthStencilState>
	{
		std::size_t operator()(const LightningGE::Renderer::DepthStencilState& state)const noexcept
		{
			std::size_t seed = 0;
			boost::hash_combine(seed, state.backFace.cmpFunc);
			boost::hash_combine(seed, state.backFace.depthFailOp);
			boost::hash_combine(seed, state.backFace.failOp);
			boost::hash_combine(seed, state.backFace.passOp);
			boost::hash_combine(seed, state.depthCmpFunc);
			boost::hash_combine(seed, state.depthTestEnable);
			boost::hash_combine(seed, state.depthWriteEnable);
			boost::hash_combine(seed, state.frontFace.cmpFunc);
			boost::hash_combine(seed, state.frontFace.depthFailOp);
			boost::hash_combine(seed, state.frontFace.failOp);
			boost::hash_combine(seed, state.frontFace.passOp);
			boost::hash_combine(seed, state.stencilEnable);
			boost::hash_combine(seed, state.stencilReadMask);
			boost::hash_combine(seed, state.stencilRef);
			boost::hash_combine(seed, state.stencilWriteMask);
			return seed;
		}
	};

	template<> struct hash<LightningGE::Renderer::PipelineState>
	{
		std::size_t operator()(const LightningGE::Renderer::PipelineState& state)const noexcept
		{
			std::size_t seed = 0;
			boost::hash_combine(seed, std::hash<LightningGE::Renderer::RasterizerState>{}(state.rasterizerState));
			boost::hash_combine(seed, std::hash<LightningGE::Renderer::BlendState>{}(state.blendState));
			boost::hash_combine(seed, std::hash<LightningGE::Renderer::DepthStencilState>{}(state.depthStencilState));
			if (state.vs)
			{
				boost::hash_combine(seed, 0x01);
				boost::hash_combine(seed, LightningGE::Renderer::Shader::Hash(state.vs->GetType(), state.vs->GetName(), state.vs->GetMacros()));
			}
			if (state.fs)
			{
				boost::hash_combine(seed, 0x02);
				boost::hash_combine(seed, LightningGE::Renderer::Shader::Hash(state.fs->GetType(), state.fs->GetName(), state.fs->GetMacros()));
			}
			if (state.gs)
			{
				boost::hash_combine(seed, 0x04);
				boost::hash_combine(seed, LightningGE::Renderer::Shader::Hash(state.gs->GetType(), state.gs->GetName(), state.gs->GetMacros()));
			}
			if (state.hs)
			{
				boost::hash_combine(seed, 0x10);
				boost::hash_combine(seed, LightningGE::Renderer::Shader::Hash(state.hs->GetType(), state.hs->GetName(), state.hs->GetMacros()));
			}
			if (state.ds)
			{
				boost::hash_combine(seed, 0x20);
				boost::hash_combine(seed, LightningGE::Renderer::Shader::Hash(state.ds->GetType(), state.ds->GetName(), state.ds->GetMacros()));
			}
			for (auto it = state.vertexComponents.cbegin();it != state.vertexComponents.cend();++it)
			{
				const auto& component = it->first;
				boost::hash_combine(seed, std::hash<LightningGE::Renderer::VertexComponent>{}(component));
				boost::hash_combine(seed, it->second);
			}
			return seed;
		}
	};
}