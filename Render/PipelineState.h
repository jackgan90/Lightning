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

#define GET_HASH_METHOD \
std::uint32_t GetHash()const\
{\
	return GetObjectHash(this);\
}

namespace Lightning
{
	namespace Render
	{
		template<typename T>
		std::uint32_t GetObjectHash(T* object)
		{
			static std::random_device rd;
			static std::mt19937 mt(rd());
			static std::uniform_int_distribution<std::uint32_t> dist(1, static_cast<std::uint32_t>(-1));
			static std::uint32_t seed = dist(mt);
			return Foundation::Utility::Hash(object, sizeof(T), seed);
		}

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
			void Reset()
			{
				rasterizerState.Reset();
				for (auto i = 0;i < MAX_RENDER_TARGET_COUNT;++i)
				{
					blendStates[i].Reset();
				}
				depthStencilState.Reset();
			}
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
PLAIN_OBJECT_HASH_SPECILIZATION(Lightning::Render::ScissorRect)
PLAIN_OBJECT_HASH_SPECILIZATION(Lightning::Render::Viewport)
PLAIN_OBJECT_HASH_SPECILIZATION(Lightning::Render::DepthStencilState)
PLAIN_OBJECT_HASH_SPECILIZATION(Lightning::Render::StencilFace)

namespace std{
	template<> struct std::hash<Lightning::Render::VertexInputLayout>
	{
		std::size_t operator()(const Lightning::Render::VertexInputLayout& state)const noexcept
		{
			static constexpr std::size_t MAX_COMPONENT_COUNT{ 32 };
			static constexpr std::size_t VERTEX_COMPONENT_SIZE = sizeof(Lightning::Render::VertexComponent);
			assert(state.componentCount < MAX_COMPONENT_COUNT);
			std::uint8_t buffer[2 + MAX_COMPONENT_COUNT * VERTEX_COMPONENT_SIZE];
			std::memset(buffer, 0, sizeof(buffer));
			buffer[0] = state.slot;
			buffer[1] = state.componentCount;
			for (auto i = 0;i < state.componentCount;++i)
			{
				std::memcpy(buffer + 2 + i * VERTEX_COMPONENT_SIZE, &state.components[i], VERTEX_COMPONENT_SIZE);
			}
			return Lightning::Foundation::Utility::Hash(buffer, sizeof(buffer), 0x12345678u);
		}
	};

	template<> struct std::hash<Lightning::Render::PipelineState>
	{
		std::size_t operator()(const Lightning::Render::PipelineState& state)const noexcept
		{
			std::uint8_t buffer[sizeof(state) + sizeof(std::size_t)];
			std::memcpy(buffer, &state, sizeof(state));
			auto inputLayoutOffset = reinterpret_cast<const std::uint8_t*>(&state.inputLayouts) - \
				reinterpret_cast<const std::uint8_t*>(&state);
			std::memset(&buffer[inputLayoutOffset], 0, sizeof(state.inputLayouts));
			*reinterpret_cast<std::size_t*>(&buffer[sizeof(state)]) = \
				std::hash<Lightning::Render::VertexInputLayout>{}(*state.inputLayouts);
			return Lightning::Foundation::Utility::Hash(buffer, sizeof(buffer), 0x12345678u);
		}
	};
}
