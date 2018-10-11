#pragma once
#include <unordered_map>
#include <cstdint>
#include <boost/functional/hash.hpp>
#include "plainobject.h"
#include "ishader.h"
#include "irendertarget.h"
#include "vertexbuffer.h"
#include "renderconstants.h"
#include "types/rect.h"

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

		struct BlendState : Foundation::PlainObject<BlendState>
		{
			bool enable{ false };
			BlendOperation colorOp{BlendOperation::ADD};
			BlendOperation alphaOp{BlendOperation::ADD};
			BlendFactor srcColorFactor{BlendFactor::SRC_ALPHA};
			BlendFactor srcAlphaFactor{BlendFactor::SRC_ALPHA};
			BlendFactor destColorFactor{BlendFactor::INV_SRC_ALPHA};
			BlendFactor destAlphaFactor{BlendFactor::INV_SRC_ALPHA};
		};

		struct RasterizerState : Foundation::PlainObject<RasterizerState>
		{
			FillMode fillMode{FillMode::SOLID};
			CullMode cullMode{CullMode::BACK};
			WindingOrder frontFace{WindingOrder::COUNTER_CLOCKWISE};

		};

		struct StencilFace : Foundation::PlainObject<StencilFace>
		{
			CmpFunc cmpFunc{CmpFunc::ALWAYS};
			StencilOp passOp{StencilOp::KEEP};
			StencilOp failOp{StencilOp::KEEP};
			StencilOp depthFailOp{StencilOp::KEEP};
		};

		struct DepthStencilState : Foundation::PlainObject<DepthStencilState>
		{
			//depth config
			bool depthTestEnable{true};
			bool depthWriteEnable{true};
			CmpFunc depthCmpFunc{CmpFunc::LESS};
			//stencil config
			bool stencilEnable{false};
			std::uint8_t stencilRef{ 0 };
			std::uint8_t stencilReadMask{0xff};
			std::uint8_t stencilWriteMask{0xff};
			StencilFace frontFace{};
			StencilFace backFace{};
		};

		struct VertexInputLayout : Foundation::PlainObject<VertexInputLayout>
		{
			std::uint8_t slot{ 0 };
			VertexComponent *components{ nullptr };
			std::uint8_t componentCount{ 0 };
		};

		struct Viewport : public Foundation::PlainObject<Viewport>
		{
			float left{ .0f };
			float top{ .0f };
			float width{ .0f };
			float height{ .0f};
		};

		struct ScissorRect : public Foundation::PlainObject<ScissorRect>
		{
			float left{ .0f };
			float top{ .0f };
			float width{ .0f };
			float height{ .0f };
		};

		struct PipelineState : Foundation::PlainObject<PipelineState>
		{
			PrimitiveType primType;
			std::uint8_t renderTargetCount;
			RasterizerState rasterizerState;
			BlendState blendStates[MAX_RENDER_TARGET_COUNT];
			DepthStencilState depthStencilState;
			IShader* vs;
			IShader* fs;
			IShader* gs;
			IShader* hs;
			IShader* ds;
			VertexInputLayout *inputLayouts;
			std::uint8_t inputLayoutCount;
			Viewport viewPort;
			ScissorRect scissorRect;
			IRenderTarget* renderTargets[MAX_RENDER_TARGET_COUNT];
		};

	}
}
PLAIN_OBJECT_HASH_SPECILIZATION(Lightning::Render::BlendState)
PLAIN_OBJECT_HASH_SPECILIZATION(Lightning::Render::RasterizerState)
PLAIN_OBJECT_HASH_SPECILIZATION(Lightning::Render::PipelineState)
PLAIN_OBJECT_HASH_SPECILIZATION(Lightning::Render::ScissorRect)
PLAIN_OBJECT_HASH_SPECILIZATION(Lightning::Render::Viewport)
PLAIN_OBJECT_HASH_SPECILIZATION(Lightning::Render::VertexInputLayout)
PLAIN_OBJECT_HASH_SPECILIZATION(Lightning::Render::DepthStencilState)
PLAIN_OBJECT_HASH_SPECILIZATION(Lightning::Render::StencilFace)
