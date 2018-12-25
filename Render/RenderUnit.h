#pragma once
#include "Container.h"
#include "Geometry.h"
#include "IMaterial.h"
#include "IRenderTarget.h"
#include "IDepthStencilBuffer.h"
#include "Transform.h"

namespace Lightning
{
	namespace Render
	{
		using namespace Foundation::Math;
		struct RenderUnit
		{
			void Reset()
			{
				geometry.Reset();
				material = nullptr;
				std::memset(renderTargets, 0, sizeof(renderTargets));
				renderTargetCount = 0;
				depthStencilBuffer = nullptr;
			}
			Geometry geometry;	//vb ib 
			IMaterial* material;	//shader material attributes
			Transform transform;		//position rotation scale
			Matrix4f viewMatrix;		//camera view matrix
			Matrix4f projectionMatrix;//camera projection matrix
			IRenderTarget* renderTargets[MAX_RENDER_TARGET_COUNT];	//render targets
			std::uint8_t renderTargetCount;	//valid render target count
			IDepthStencilBuffer* depthStencilBuffer; //depth stencil buffer for this draw
		};
		static_assert(std::is_pod<RenderUnit>::value, "RenderUnit is not a POD type.");
		//RenderQueue is a very important concept.Every drawable object will encode there data to a RenderUnit struct.
		//After enqueue,the nodes remain unchangeable.The responsibility for render pass
		//is to read unit data from the queue and carry out different render algorithms.The data in this
		//queue won't be cleared until next time the renderer renders to the same swap chain,which ensures the 
		//render resource validation during this frame(render resources won't be deleted because a render unit
		//holds all shared_ptr referring to them).
		using RenderQueue = Foundation::Container::Vector<RenderUnit>;
	}
}
