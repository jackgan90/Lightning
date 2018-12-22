#pragma once
#include "RenderConstants.h"
#include "Quaternion.h"
#include "Color.h"
#include "IDrawable.h"
#include "Texture/ITexture.h"
#include "Texture/Sampler.h"
#include "IShader.h"

namespace Lightning
{
	namespace Scene
	{
		using Foundation::Math::Vector3f;
		using Foundation::Math::Quaternionf;
		using Render::Color32;
		using Render::PrimitiveType;
		using Render::ITexture;
		using Render::SamplerState;
		using Render::IShader;
		enum class PrimitiveShape
		{
			CUBE,
			CYLINDER,
			HEMISPHERE,
			SPHERE
		};

		struct IPrimitive : IDrawable
		{
			virtual PrimitiveType INTERFACECALL GetPrimitiveType()const = 0;
			virtual void INTERFACECALL SetWorldPosition(const Vector3f& pos) = 0;
			virtual Vector3f INTERFACECALL GetWorldPosition()const = 0;
			virtual void INTERFACECALL SetWorldRotation(const Quaternionf& rot) = 0;
			virtual Quaternionf INTERFACECALL GetWorldRotation()const = 0;
			virtual Color32 INTERFACECALL GetColor()const = 0;
			virtual void INTERFACECALL GetColor(float& a, float& r, float& g, float& b) = 0;
			virtual void INTERFACECALL SetColor(const Color32& color) = 0;
			//color is in ARGB order
			virtual void INTERFACECALL SetColor(std::uint32_t color) = 0;
			virtual void INTERFACECALL SetColor(float a, float r, float g, float b) = 0;
			virtual void INTERFACECALL SetTransparency(std::uint8_t transparency) = 0;
			virtual void INTERFACECALL SetTransparency(float transparency) = 0;
			virtual void INTERFACECALL SetTexture(const char* name, ITexture* texture) = 0;
			virtual void INTERFACECALL SetSamplerState(const char* name, const SamplerState& state) = 0;
			virtual void INTERFACECALL SetShader(IShader* shader) = 0;
		};
	}
}
