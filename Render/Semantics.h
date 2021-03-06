#pragma once
#include <cstdint>

namespace Lightning
{
	namespace Render
	{
		using SemanticIndex = unsigned int;
		//Generally speaking,all of the render semantics can be divided into two categories.
		//One are those that are input to graphics pipeline(such as vertex position, normal, tangent etc).
		//Others are those that correspond to uniform(constants) in shader pipeline(such as delta frame time etc)
		enum RenderSemantics
		{
			UNKNOWN = 0,
			POSITION,
			NORMAL,
			TANGENT,
			TEXCOORD0,
			TEXCOORD1,
			TEXCOORD2,
			TEXCOORD3,
			TEXCOORD4,
			TEXCOORD5,
			TEXCOORD6,
			TEXCOORD7,
			CAMERA_POSITION,
			FRAME_COUNT,
			FRAME_DELTA_TIME,
			WVP,
		};

		struct SemanticItem
		{
			RenderSemantics semantic;
			const char* name;
		};

		//These semantics are input assembly semantics
		const SemanticItem PipelineInputSemantics[] = {
			{RenderSemantics::POSITION, "POSITION"},
			{RenderSemantics::NORMAL, "NORMAL"},
			{RenderSemantics::TANGENT, "TANGENT"},
			{RenderSemantics::TEXCOORD0, "TEXCOORD0"},
			{RenderSemantics::TEXCOORD1, "TEXCOORD1"},
			{RenderSemantics::TEXCOORD2, "TEXCOORD2"},
			{RenderSemantics::TEXCOORD3, "TEXCOORD3"},
			{RenderSemantics::TEXCOORD4, "TEXCOORD4"},
			{RenderSemantics::TEXCOORD5, "TEXCOORD5"},
			{RenderSemantics::TEXCOORD6, "TEXCOORD6"},
			{RenderSemantics::TEXCOORD7, "TEXCOORD7"},
		};

		//These semantics are uniform(constant buffer variable) semantics
		const SemanticItem UniformSemantics[] = {
			{RenderSemantics::WVP, "wvp"},
			{RenderSemantics::CAMERA_POSITION, "camera_pos"},
			{RenderSemantics::FRAME_COUNT, "frame_count"},
			{RenderSemantics::FRAME_DELTA_TIME, "frame_delta_time"},
		};
	}
}