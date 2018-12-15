#pragma once
#include <cstdint>

namespace Lightning
{
	namespace Render
	{
		using SemanticIndex = unsigned int;
		enum class RenderSemantics : std::uint8_t
		{
			UNKNOWN,
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
			LIGHT_POSITION,
			WVP,
		};

		struct SemanticItem
		{
			RenderSemantics semantic;
			char* name;
		};

		const SemanticItem EngineSemantics[] = {
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
			{RenderSemantics::CAMERA_POSITION, "CAMERA_POSITION"},
			{RenderSemantics::FRAME_COUNT, "FRAME_COUNT"},
			{RenderSemantics::FRAME_DELTA_TIME, "FRAME_DELTA_TIME"},
			{RenderSemantics::LIGHT_POSITION, "LIGHT_POSITION"},
			{RenderSemantics::WVP, "WVP"},
		};
	}
}