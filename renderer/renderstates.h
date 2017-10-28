#pragma once
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
		
		struct BlendState
		{
			BlendState():enable(false), colorOp(BLEND_ADD) ,alphaOp(BLEND_ADD)
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
			IRenderTarget* renderTarget;
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
		};

		struct PipelineState
		{
			RasterizerState rasterizerState;
			BlendState blendState;
			DepthStencilState depthStencilState;
		};
	}
}