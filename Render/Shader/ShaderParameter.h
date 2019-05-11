#pragma once
#include "boost/variant.hpp"
#include "Vector.h"
#include "Matrix.h"
#include "Texture/ITexture.h"
#include "Texture/Sampler.h"

namespace Lightning
{
	namespace Render
	{
		using namespace Foundation::Math;

		enum class ShaderParameterType
		{
			UNKNOWN = 0,
			FLOAT,
			FLOAT2,
			FLOAT3,
			FLOAT4,
			MATRIX4X4F,
			TEXTURE,
			SAMPLER,
		};

		class ShaderParameter
		{
		public:
			template<typename ValueType>
			ShaderParameter(const std::string& name, ValueType&& value)
				: mName(name), mValue(std::forward<ValueType>(value))
			{
				mType = static_cast<ShaderParameterType>(mValue.which() + 1);
			}
			
			std::string GetName()const
			{
				return mName;
			}
			ShaderParameterType GetType()const
			{
				return mType;
			}

			const void* Buffer(std::size_t& size)const
			{
				ValueVisitor visitor(mValue);
				auto buffer = mValue.apply_visitor(visitor);
				size = visitor.size;
				return buffer;
			}
		private:
			using Variant = 
			//The order of template parameters must match that defined in ShaderParameterType
			boost::variant
			<	float, 
				Vector2f, 
				Vector3f, 
				Vector4f, 
				Matrix4f, 
				ITexture*, 
				SamplerState
			>;
			struct ValueVisitor : public boost::static_visitor<const void*>
			{
				ValueVisitor(const Variant& var):variant(var){}
				std::size_t size{ 0 };
				const Variant& variant;
				template<typename ValueType>
				const void* operator()(const ValueType& value)
				{
					size = sizeof(ValueType);
					return &boost::get<ValueType>(variant);
				}
			};
			ShaderParameterType mType;
			std::string mName;
			Variant mValue;
		};
	}
}
