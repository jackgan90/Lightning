#pragma once
#include "rendererexportdef.h"
#include "geometry.h"
#include "material.h"
#include "transform.h"

namespace LightningGE
{
	namespace Render
	{
		struct LIGHTNINGGE_RENDER_API RenderItem
		{
			SharedGeometryPtr geometry;	//vb ib 
			SharedMaterialPtr material;	//shader material attributes
			Transform transform;		//position rotation scale
			Matrix4x4f viewMatrix;		//camera view matrix
			Matrix4x4f projectionMatrix;//camera projection matrix
		};
	}
}
