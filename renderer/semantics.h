#pragma once

namespace LightningGE
{
	namespace Renderer
	{
		typedef unsigned int SemanticIndex;
		enum RenderSemantics
		{
			POSITION,
			NORMAL,
			TANGENT,
			CAMERA_POSITION,
			FRAME_COUNT,
			FRAME_DELTA_TIME,
			LIGHT_POSITION
		};

		struct SemanticItem
		{
			RenderSemantics semantic;
			char name[32];
		};

		const SemanticItem EngineSemantics[] = {
			{POSITION, "POSITION"},
			{NORMAL, "NORMAL"},
			{TANGENT, "TANGENT"},
			{CAMERA_POSITION, "CAMERA_POSITION"},
			{FRAME_COUNT, "FRAME_COUNT"},
			{FRAME_DELTA_TIME, "FRAME_DELTA_TIME"},
			{LIGHT_POSITION, "LIGHT_POSITION"},
		};
	}
}