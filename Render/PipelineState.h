#pragma once
#include <unordered_map>
#include <cstdint>
#include <boost/functional/hash.hpp>
#include <cassert>
#include "PlainObject.h"
#include "IShader.h"
#include "IRenderTarget.h"
#include "VertexBuffer.h"
#include "RenderConstants.h"
#include "types/Rect.h"

namespace Lightning
{
	namespace Render
	{
		enum class BlendOperation : std::uint8_t
		{
			ADD,
			SUBTRACT,
			REVERSE_SUBTRACT,
			MIN,
			MAX
		};

		enum class BlendFactor : std::uint8_t
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
		

		enum class FillMode : std::uint8_t
		{
			WIREFRAME,
			SOLID
		};

		enum class CullMode : std::uint8_t
		{
			NONE,
			FRONT,
			BACK
		};

		enum class WindingOrder : std::uint8_t
		{
			COUNTER_CLOCKWISE,
			CLOCKWISE
		};


		enum class CmpFunc : std::uint8_t
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

		enum class StencilOp : std::uint8_t
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

		struct BlendState
		{
			GET_HASH_METHOD
			void Reset()
			{
				enable = false;
				colorOp = BlendOperation::ADD;
				alphaOp = BlendOperation::ADD;
				srcColorFactor = BlendFactor::SRC_ALPHA;
				srcAlphaFactor = BlendFactor::SRC_ALPHA;
				destColorFactor = BlendFactor::INV_SRC_ALPHA;
				destAlphaFactor = BlendFactor::INV_SRC_ALPHA;
			}
			bool enable;
			BlendOperation colorOp;
			BlendOperation alphaOp;
			BlendFactor srcColorFactor;
			BlendFactor srcAlphaFactor;
			BlendFactor destColorFactor;
			BlendFactor destAlphaFactor;
		};
		static_assert(std::is_pod<BlendState>::value, "BlendState is not a POD type.");

		struct RasterizerState
		{
			GET_HASH_METHOD
			void Reset()
			{
				fillMode = FillMode::SOLID;
				cullMode = CullMode::BACK;
				frontFace = WindingOrder::COUNTER_CLOCKWISE;
			}
			FillMode fillMode;
			CullMode cullMode;
			WindingOrder frontFace;
		};
		static_assert(std::is_pod<RasterizerState>::value, "RasterizerState is not a POD type.");

		struct StencilFace
		{
			GET_HASH_METHOD
			void Reset()
			{
				cmpFunc = CmpFunc::ALWAYS;
				passOp = StencilOp::KEEP;
				failOp = StencilOp::KEEP;
				depthFailOp = StencilOp::KEEP;
			}
			CmpFunc cmpFunc;
			StencilOp passOp;
			StencilOp failOp;
			StencilOp depthFailOp;
		};
		static_assert(std::is_pod<StencilFace>::value, "StencilFace is not a POD type.");

		struct DepthStencilState
		{
			GET_HASH_METHOD
			void Reset()
			{
				depthTestEnable = true;
				depthWriteEnable = true;
				depthCmpFunc = CmpFunc::LESS;
				stencilEnable = false;
				stencilRef = 0;
				stencilReadMask = 0xff;
				stencilWriteMask = 0xff;
				frontFace.Reset();
				backFace.Reset();
				bufferFormat = RenderFormat::UNDEFINED;
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
			RenderFormat bufferFormat;
		};
		static_assert(std::is_pod<DepthStencilState>::value, "DepthStencilState is not a POD type.");

		struct VertexInputLayout
		{
			void Reset()
			{
				slot = 0;
				componentCount = 0;
				components = nullptr;
				//Don't reset each components,because componentCount indicate the number of valid components is 0
			}
			std::size_t slot;
			VertexComponent* components;
			//Users of this struct should always check componentCount to ensure validation of components
			std::size_t componentCount;
		};
		static_assert(std::is_pod<VertexInputLayout>::value, "VertexInputLayout is not a POD type.");

		struct Viewport
		{
			GET_HASH_METHOD
			void Reset()
			{
				left = top = width = height = .0f;
			}
			float left;
			float top;
			float width;
			float height;
		};
		static_assert(std::is_pod<Viewport>::value, "Viewport is not a POD type.");

		struct ScissorRect
		{
			GET_HASH_METHOD
			void Reset()
			{
				left = top = width = height = 0;
			}
			long left;
			long top;
			long width;
			long height;
		};
		static_assert(std::is_pod<ScissorRect>::value, "ScissorRect is not a POD type.");

		struct RenderTargetBlendState
		{
			std::shared_ptr<IRenderTarget> renderTarget;
			BlendState blendState;
		};

		struct PipelineState
		{
			void Reset()
			{
				vs = fs = gs = hs = ds = nullptr;

				primType = PrimitiveType::TRIANGLE_LIST;
				rasterizerState.Reset();
				depthStencilState.Reset();

				inputLayouts.clear();
				renderTargetBlendStates.clear();
			}
			std::shared_ptr<IShader> vs;
			std::shared_ptr<IShader> fs;
			std::shared_ptr<IShader> gs;
			std::shared_ptr<IShader> hs;
			std::shared_ptr<IShader> ds;
			PrimitiveType primType;
			RasterizerState rasterizerState;
			DepthStencilState depthStencilState;
			//Don't try to reorder the following fields.array fields must be put to the end of struct
			std::vector<VertexInputLayout> inputLayouts;
			std::vector<RenderTargetBlendState> renderTargetBlendStates;
		};

	}
}
PLAIN_OBJECT_HASH_SPECILIZATION(Lightning::Render::BlendState)
PLAIN_OBJECT_HASH_SPECILIZATION(Lightning::Render::RasterizerState)
PLAIN_OBJECT_HASH_SPECILIZATION(Lightning::Render::ScissorRect)
PLAIN_OBJECT_HASH_SPECILIZATION(Lightning::Render::Viewport)
PLAIN_OBJECT_HASH_SPECILIZATION(Lightning::Render::DepthStencilState)
PLAIN_OBJECT_HASH_SPECILIZATION(Lightning::Render::StencilFace)

namespace std{
	template<> struct std::hash<Lightning::Render::VertexInputLayout>
	{
		std::size_t operator()(const Lightning::Render::VertexInputLayout& layout)const noexcept
		{
			std::size_t seed{ 0 };
			boost::hash_combine(seed, layout.slot);
			boost::hash_combine(seed, layout.componentCount);
			for (auto i = 0;i < layout.componentCount;++i)
			{
				boost::hash_combine(seed, layout.components[i].GetHash());
			}
			return seed;
		}
	};

	template<> struct std::hash<Lightning::Render::PipelineState>
	{
		std::size_t operator()(const Lightning::Render::PipelineState& state)const noexcept
		{
			std::size_t hashValue{ 0x12345678u };
			boost::hash_combine(hashValue, state.primType);
			boost::hash_combine(hashValue, state.rasterizerState.GetHash());
			boost::hash_combine(hashValue, state.depthStencilState.GetHash());
			for (auto i = 0;i < state.inputLayouts.size();++i)
			{
				const auto& inputLayout = state.inputLayouts[i];
				boost::hash_combine(hashValue, std::hash<Lightning::Render::VertexInputLayout>{}(inputLayout));
			}
			for (auto i = 0;i < state.renderTargetBlendStates.size();++i)
			{
				const auto& renderTarget = state.renderTargetBlendStates[i].renderTarget;
				const auto& blendState = state.renderTargetBlendStates[i].blendState;
				boost::hash_combine(hashValue, blendState.GetHash());
				//FIXME : render target should have its own descriptor,now only use the attached texture's render format to calculate the hash
				boost::hash_combine(hashValue, renderTarget->GetTexture()->GetRenderFormat());
			}
			if (state.vs)
			{
				boost::hash_combine(hashValue, state.vs->GetHash());
			}
			if (state.fs)
			{
				boost::hash_combine(hashValue, state.fs->GetHash());
			}
			if (state.gs)
			{
				boost::hash_combine(hashValue, state.gs->GetHash());
			}
			if (state.hs)
			{
				boost::hash_combine(hashValue, state.hs->GetHash());
			}
			if (state.ds)
			{
				boost::hash_combine(hashValue, state.ds->GetHash());
			}
			return hashValue;
		}
	};
}
