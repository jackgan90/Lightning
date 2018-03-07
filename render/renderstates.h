#pragma once
#include <functional>
#include <unordered_map>
#include <cstdint>
#include <boost/functional/hash.hpp>
#include "ishader.h"
#include "irendertarget.h"
#include "vertexbuffer.h"
#include "types/rect.h"

namespace LightningGE
{
	namespace Render
	{
		enum class BlendOperation
		{
			ADD,
			SUBTRACT,
			REVERSE_SUBTRACT,
			MIN,
			MAX
		};

		enum class BlendFactor
		{
			ZERO,
			ONE,
			SRC_COLOR,	
			INV_SRC_COLOR,
			SRC_ALPHA,
			INV_SRC_ALPHA,
			DEST_COLOR,
			INV_DEST_COLOR,
			DEST_ALPHA,
			INV_DEST_ALPHA,
			//TODO there's more advanced blend factor in recent graphics API ,need to add them in case of use
		};
		
		struct BlendState
		{
			BlendState():enable(false), colorOp(BlendOperation::ADD) ,alphaOp(BlendOperation::ADD)
				, srcColorFactor(BlendFactor::SRC_ALPHA) ,srcAlphaFactor(BlendFactor::SRC_ALPHA)
				, destColorFactor(BlendFactor::INV_SRC_ALPHA), destAlphaFactor(BlendFactor::INV_SRC_ALPHA)
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

		enum class FillMode
		{
			WIREFRAME,
			SOLID
		};

		enum class CullMode
		{
			NONE,
			FRONT,
			BACK
		};

		enum class FrontFaceWindingOrder
		{
			COUNTER_CLOCKWISE,
			CLOCKWISE
		};

		struct RasterizerState
		{
			RasterizerState() :fillMode(FillMode::SOLID), cullMode(CullMode::BACK), frontFaceWindingOrder(FrontFaceWindingOrder::COUNTER_CLOCKWISE)
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

		enum class CmpFunc
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

		enum class StencilOp
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
			StencilFace() : cmpFunc(CmpFunc::ALWAYS), passOp(StencilOp::KEEP), failOp(StencilOp::KEEP), depthFailOp(StencilOp::KEEP)
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
			DepthStencilState() : depthTestEnable(true), depthWriteEnable(true), depthCmpFunc(CmpFunc::LESS)
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

		struct VertexInputLayout
		{
			std::uint8_t slot;
			std::vector<VertexComponent> components;
		};

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
			VertexInputLayout inputLayout;

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

				if (inputLayout.slot != state.inputLayout.slot)
				{
					return false;
				}

				if (inputLayout.components.size() != state.inputLayout.components.size())
				{
					return false;
				}

				for (std::size_t i = 0; i < inputLayout.components.size();++i)
				{
					if (inputLayout.components[i] != state.inputLayout.components[i])
						return false;
				}

				return true;
			}

			bool operator!=(const PipelineState& state)const noexcept
			{
				return !(*this == state);
			}
		};

		struct Viewport : public RectF
		{
		};

		struct ScissorRect : public RectF
		{

		};
	}
}

namespace std
{
	template<> struct hash<LightningGE::Render::RasterizerState>
	{
		std::size_t operator()(const LightningGE::Render::RasterizerState& state)const noexcept
		{
			std::size_t seed = 0;
			boost::hash_combine(seed, state.cullMode);
			boost::hash_combine(seed, state.fillMode);
			boost::hash_combine(seed, state.frontFaceWindingOrder);
			return seed;
		}
	};

	template<> struct hash<LightningGE::Render::BlendState>
	{
		std::size_t operator()(const LightningGE::Render::BlendState& state)const noexcept
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

	template<> struct hash<LightningGE::Render::DepthStencilState>
	{
		std::size_t operator()(const LightningGE::Render::DepthStencilState& state)const noexcept
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

	template<> struct hash<LightningGE::Render::VertexInputLayout>
	{
		std::size_t operator()(const LightningGE::Render::VertexInputLayout& layout)const noexcept
		{
			std::size_t seed = 0;
			boost::hash_combine(seed, layout.slot);
			boost::hash_combine(seed, layout.components.size());
			for (const auto& component : layout.components)
			{
				boost::hash_combine(seed, std::hash<LightningGE::Render::VertexComponent>{}(component));
			}
			return seed;
		}
	};

	template<> struct hash<LightningGE::Render::PipelineState>
	{
		std::size_t operator()(const LightningGE::Render::PipelineState& state)const noexcept
		{
			std::size_t seed = 0;
			boost::hash_combine(seed, std::hash<LightningGE::Render::RasterizerState>{}(state.rasterizerState));
			boost::hash_combine(seed, std::hash<LightningGE::Render::BlendState>{}(state.blendState));
			boost::hash_combine(seed, std::hash<LightningGE::Render::DepthStencilState>{}(state.depthStencilState));
			if (state.vs)
			{
				boost::hash_combine(seed, 0x01);
				boost::hash_combine(seed, LightningGE::Render::Shader::Hash(state.vs->GetType(), state.vs->GetName(), state.vs->GetMacros()));
			}
			if (state.fs)
			{
				boost::hash_combine(seed, 0x02);
				boost::hash_combine(seed, LightningGE::Render::Shader::Hash(state.fs->GetType(), state.fs->GetName(), state.fs->GetMacros()));
			}
			if (state.gs)
			{
				boost::hash_combine(seed, 0x04);
				boost::hash_combine(seed, LightningGE::Render::Shader::Hash(state.gs->GetType(), state.gs->GetName(), state.gs->GetMacros()));
			}
			if (state.hs)
			{
				boost::hash_combine(seed, 0x10);
				boost::hash_combine(seed, LightningGE::Render::Shader::Hash(state.hs->GetType(), state.hs->GetName(), state.hs->GetMacros()));
			}
			if (state.ds)
			{
				boost::hash_combine(seed, 0x20);
				boost::hash_combine(seed, LightningGE::Render::Shader::Hash(state.ds->GetType(), state.ds->GetName(), state.ds->GetMacros()));
			}

			boost::hash_combine(seed, std::hash<LightningGE::Render::VertexInputLayout>{}(state.inputLayout));
			return seed;
		}
	};
}