#pragma once
#include <d3d12.h>
#include "iblendstate.h"

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
		};
	}
}