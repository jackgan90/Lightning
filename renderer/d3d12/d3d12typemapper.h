#pragma once
#include <d3d12.h>
#include "iblendstate.h"
#include "idepthstencilstate.h"
#include "irasterizerstate.h"

namespace LightningGE
{
	namespace Renderer
	{
		class D3D12TypeMapper
		{
		public:
			static D3D12_BLEND_OP MapBlendOp(const BlendOperation& op)
			{
				switch (op)
				{
				case BLEND_ADD:
					return D3D12_BLEND_OP_ADD;
				case BLEND_SUBTRACT:
					return D3D12_BLEND_OP_SUBTRACT;
				case BLEND_REVERSE_SUBTRACT:
					return D3D12_BLEND_OP_REV_SUBTRACT;
				case BLEND_MIN:
					return D3D12_BLEND_OP_MIN;
				case BLEND_MAX:
					return D3D12_BLEND_OP_MAX;
				default:
					return D3D12_BLEND_OP_ADD;
				}
			}

			static D3D12_BLEND MapBlendFactor(const BlendFactor& factor)
			{
				switch (factor)
				{
				case BLEND_ZERO:
					return D3D12_BLEND_ZERO;
				case BLEND_ONE:
					return D3D12_BLEND_ONE;
				case BLEND_SRC_COLOR:
					return D3D12_BLEND_SRC_COLOR;
				case BLEND_INV_SRC_COLOR:
					return D3D12_BLEND_INV_SRC_COLOR;
				case BLEND_SRC_ALPHA:
					return D3D12_BLEND_SRC_ALPHA;
				case BLEND_INV_SRC_ALPHA:
					return D3D12_BLEND_INV_SRC_ALPHA;
				case BLEND_DEST_COLOR:
					return D3D12_BLEND_DEST_COLOR;
				case BLEND_INV_DEST_COLOR:
					return D3D12_BLEND_INV_DEST_COLOR;
				case BLEND_DEST_ALPHA:
					return D3D12_BLEND_DEST_ALPHA;
				case BLEND_INV_DEST_ALPHA:
					return D3D12_BLEND_INV_DEST_ALPHA;
				default:
					return D3D12_BLEND_ZERO;
				}
			}

			static D3D12_COMPARISON_FUNC MapCmpFunc(const CmpFunc& func)
			{
				switch (func)
				{
				case NEVER:
					return D3D12_COMPARISON_FUNC_NEVER;
				case LESS:
					return D3D12_COMPARISON_FUNC_LESS;
				case EQUAL:
					return D3D12_COMPARISON_FUNC_EQUAL;
				case LESS_EQUAL:
					return D3D12_COMPARISON_FUNC_LESS_EQUAL;
				case GREATER:
					return D3D12_COMPARISON_FUNC_GREATER;
				case NOT_EQUAL:
					return D3D12_COMPARISON_FUNC_NOT_EQUAL;
				case GREATER_EQUAL:
					return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
				case ALWAYS:
					return D3D12_COMPARISON_FUNC_ALWAYS;
				default:
					return D3D12_COMPARISON_FUNC_LESS;
				}
			}

			static D3D12_STENCIL_OP MapStencilOp(const StencilOp& op)
			{
				switch (op)
				{
				case KEEP:
					return D3D12_STENCIL_OP_KEEP;
				case ZERO:
					return D3D12_STENCIL_OP_ZERO;
				case REPLACE:
					return D3D12_STENCIL_OP_REPLACE;
				case INCREASE_CLAMP:
					return D3D12_STENCIL_OP_INCR_SAT;
				case DECREASE_CLAMP:
					return D3D12_STENCIL_OP_DECR_SAT;
				case INVERT:
					return D3D12_STENCIL_OP_INVERT;
				case INCREASE_WRAP:
					return D3D12_STENCIL_OP_INCR;
				case DECREASE_WRAP:
					return D3D12_STENCIL_OP_DECR;
				default:
					return D3D12_STENCIL_OP_KEEP;
				}
			}

			static D3D12_FILL_MODE MapFillMode(const FillMode& mode)
			{
				switch (mode)
				{
				case FILLMODE_SOLID:
					return D3D12_FILL_MODE_SOLID;
				case FILLMODE_WIREFRAME:
					return D3D12_FILL_MODE_WIREFRAME;
				default:
					return D3D12_FILL_MODE_SOLID;
				}
			}

			static D3D12_CULL_MODE MapCullMode(const CullMode& mode)
			{
				switch (mode)
				{
				case CULLMODE_NONE:
					return D3D12_CULL_MODE_NONE;
				case CULLMODE_FRONT:
					return D3D12_CULL_MODE_FRONT;
				case CULLMODE_BACK:
					return D3D12_CULL_MODE_BACK;
				default:
					return D3D12_CULL_MODE_BACK;
				}
			}
		};
	}
}