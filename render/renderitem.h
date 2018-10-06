#pragma once
#include "rendererexportdef.h"
#include "geometry.h"
#include "material.h"
#include "transform.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::Math::Transform;
		struct LIGHTNING_RENDER_API RenderItem
		{
			RenderItem():renderTargets(1){}
			SharedGeometryPtr geometry;	//vb ib 
			SharedMaterialPtr material;	//shader material attributes
			Transform transform;		//position rotation scale
			Matrix4f viewMatrix;		//camera view matrix
			Matrix4f projectionMatrix;//camera projection matrix
			std::uint8_t renderTargets;
		};
	}
}
