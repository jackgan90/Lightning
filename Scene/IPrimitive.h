#pragma once
#include "RenderConstants.h"
#include "Quaternion.h"
#include "Color.h"
#include "IDrawable.h"
#include "Texture/ITexture.h"

namespace Lightning
{
	namespace Scene
	{
		using Foundation::Math::Vector3f;
		using Foundation::Math::Quaternionf;
		using Render::Color32;
		using Render::PrimitiveType;
		using Render::ITexture;
		enum class PrimitiveShape
		{
			CUBE,
			CYLINDER,
			HEMISPHERE,
			SPHERE
		};

		class IPrimitive : public IDrawable
		{
		public:
			virtual PrimitiveType GetPrimitiveType()const = 0;
			virtual void SetWorldPosition(const Vector3f& pos) = 0;
			virtual Vector3f GetWorldPosition()const = 0;
			virtual void SetWorldRotation(const Quaternionf& rot) = 0;
			virtual Quaternionf GetWorldRotation()const = 0;
			virtual Color32 GetColor()const = 0;
			virtual void GetColor(float& a, float& r, float& g, float& b) = 0;
			virtual void SetColor(const Color32& color) = 0;
			//color is in ARGB order
			virtual void SetColor(std::uint32_t color) = 0;
			virtual void SetColor(float a, float r, float g, float b) = 0;
			virtual void SetTransparency(std::uint8_t transparency) = 0;
			virtual void SetTransparency(float transparency) = 0;
			virtual void SetTexture(ITexture* texture) = 0;
		};
	}
}
