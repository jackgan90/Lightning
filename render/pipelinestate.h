#pragma once
#include <functional>
#include <unordered_map>
#include <cstdint>
#include <boost/functional/hash.hpp>
#include "ishader.h"
#include "irendertarget.h"
#include "vertexbuffer.h"
#include "renderconstants.h"
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
			{
			}
			bool enable;
			BlendOperation colorOp;
			BlendOperation alphaOp;
			BlendFactor srcColorFactor;
			BlendFactor srcAlphaFactor;
			BlendFactor destColorFactor;
			BlendFactor destAlphaFactor;

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

		enum class WindingOrder
		{
			COUNTER_CLOCKWISE,
			CLOCKWISE
		};

		struct RasterizerState
		{
			RasterizerState() :fillMode(FillMode::SOLID), cullMode(CullMode::BACK), frontFace(WindingOrder::COUNTER_CLOCKWISE)
			{

			}
			FillMode fillMode;
			CullMode cullMode;
			WindingOrder frontFace;

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

				if (frontFace != state.frontFace)
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
			std::uint8_t stencilRef;
			std::uint8_t stencilReadMask;
			std::uint8_t stencilWriteMask;
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

		struct Viewport : public RectF
		{
		};

		struct ScissorRect : public RectF
		{
		};

		struct PipelineState
		{
			PrimitiveType primType;
			std::uint8_t outputRenderTargetCount;
			RasterizerState rasterizerState;
			BlendState blendStates[MAX_RENDER_TARGET_COUNT];
			DepthStencilState depthStencilState;
			IShader* vs;
			IShader* fs;
			IShader* gs;
			IShader* hs;
			IShader* ds;
			std::vector<VertexInputLayout> inputLayouts;
			Viewport viewPort;
			ScissorRect scissorRect;

			bool operator==(const PipelineState& state)const noexcept
			{
				if (primType != state.primType)
				{
					return false;
				}

				if (outputRenderTargetCount != state.outputRenderTargetCount)
				{
					return false;
				}

				if (rasterizerState != state.rasterizerState)
				{
					return false;
				}

				for (std::uint8_t i = 0;i < outputRenderTargetCount;++i)
				{
					if (blendStates[i] != state.blendStates[i])
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

				if (inputLayouts.size() != state.inputLayouts.size())
				{
					return false;
				}
				
				for (std::size_t i = 0;i < inputLayouts.size();++i)
				{
					if (inputLayouts[i].slot != state.inputLayouts[i].slot)
					{
						return false;
					}
					if (inputLayouts[i].components.size() != state.inputLayouts[i].components.size())
					{
						return false;
					}
					for (std::size_t j = 0;j < inputLayouts[i].components.size();++j)
					{
						if (inputLayouts[i].components[j] != state.inputLayouts[i].components[j])
							return false;
					}
				}

				if (viewPort != state.viewPort)
				{
					return false;
				}

				if (scissorRect != state.scissorRect)
				{
					return false;
				}

				return true;
			}

			bool operator!=(const PipelineState& state)const noexcept
			{
				return !(*this == state);
			}
		};

	}
}

namespace std
{
	template<typename T>
	std::size_t PipelineHash(const T& object)
	{
		return std::hash<T>{}(object);
	}

	template<typename T>
	std::size_t PipelineHash(const T* pObject)
	{
		return std::hash<T>{}(*oObject);
	}

	template<> struct hash<LightningGE::Render::RasterizerState>
	{
		std::size_t operator()(const LightningGE::Render::RasterizerState& state)const noexcept
		{
			std::size_t seed = 0;
			boost::hash_combine(seed, state.cullMode);
			boost::hash_combine(seed, state.fillMode);
			boost::hash_combine(seed, state.frontFace);
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
				boost::hash_combine(seed, PipelineHash(component));
			}
			return seed;
		}
	};
	
	
	template<> struct hash<LightningGE::Render::Viewport>
	{
		std::size_t operator()(const LightningGE::Render::Viewport& viewPort)const noexcept
		{
			std::size_t seed = 0;
			boost::hash_combine(seed, viewPort.left());
			boost::hash_combine(seed, viewPort.right());
			boost::hash_combine(seed, viewPort.top());
			boost::hash_combine(seed, viewPort.bottom());
			return seed;
		}
	};

	template<> struct hash<LightningGE::Render::ScissorRect>
	{
		std::size_t operator()(const LightningGE::Render::ScissorRect& scissorRect)const noexcept
		{
			std::size_t seed = 0;
			boost::hash_combine(seed, scissorRect.left());
			boost::hash_combine(seed, scissorRect.right());
			boost::hash_combine(seed, scissorRect.top());
			boost::hash_combine(seed, scissorRect.bottom());
			return seed;
		}
	};

	template<> struct hash<LightningGE::Render::PipelineState>
	{
		std::size_t operator()(const LightningGE::Render::PipelineState& state)const noexcept
		{
			std::size_t seed = 0;
			boost::hash_combine(seed, state.primType);
			boost::hash_combine(seed, state.outputRenderTargetCount);
			boost::hash_combine(seed, PipelineHash(state.rasterizerState));
			for (std::uint8_t i = 0;i < state.outputRenderTargetCount;++i)
			{
				boost::hash_combine(seed, PipelineHash(state.blendStates[i]));
			}
			boost::hash_combine(seed, PipelineHash(state.depthStencilState));
			if (state.vs)
			{
				boost::hash_combine(seed, 0x01);
				boost::hash_combine(seed, PipelineHash(state.vs));
			}
			if (state.fs)
			{
				boost::hash_combine(seed, 0x02);
				boost::hash_combine(seed, PipelineHash(state.fs));
			}
			if (state.gs)
			{
				boost::hash_combine(seed, 0x04);
				boost::hash_combine(seed, PipelineHash(state.gs));
			}
			if (state.hs)
			{
				boost::hash_combine(seed, 0x10);
				boost::hash_combine(seed, PipelineHash(state.hs));
			}
			if (state.ds)
			{
				boost::hash_combine(seed, 0x20);
				boost::hash_combine(seed, PipelineHash(state.ds));
			}
			
			boost::hash_combine(seed, state.inputLayouts.size());
			for (const auto& inputLayout : state.inputLayouts)
			{
				boost::hash_combine(seed, PipelineHash(inputLayout));
			}

			boost::hash_combine(seed, PipelineHash(state.viewPort));
			boost::hash_combine(seed, PipelineHash(state.scissorRect));
			return seed;
		}
	};
}