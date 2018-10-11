#pragma once
#include "rendererexportdef.h"
#include "container.h"
#include "geometry.h"
#include "material.h"
#include "transform.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::Math::Transform;
		struct LIGHTNING_RENDER_API RenderNode
		{
			RenderNode():renderTargets(1){}
			SharedGeometryPtr geometry;	//vb ib 
			SharedMaterialPtr material;	//shader material attributes
			Transform transform;		//position rotation scale
			Matrix4f viewMatrix;		//camera view matrix
			Matrix4f projectionMatrix;//camera projection matrix
			std::uint8_t renderTargets;
		};
		//RenderQueue is a very important concept.Every drawable object will encode there data to a RenderNode struct.
		//After enqueue,the nodes remain unchangeable.The responsibility for render pass
		//is to read node data from the queue and carry out different render algorithms.The data in this
		//queue won't be cleared until next time the renderer renders to the same swap chain,which ensures the 
		//render resource validation during this frame(render resources won't be deleted because a render node
		//holds all shared_ptr referring to them).
		using RenderQueue = container::vector<RenderNode>;
	}
}
