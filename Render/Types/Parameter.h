#pragma once
#include <string>
#include "boost/variant.hpp"
#include "Vector.h"
#include "Matrix.h"
#include "ITexture.h"
#include "Sampler.h"

namespace Lightning
{
	namespace Render
	{
		enum class ParameterType
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

		class Parameter
		{
		public:
			template<typename ValueType>
			Parameter(const std::string& name, ValueType&& value)
				: mName(name), mValue(std::forward<ValueType>(value))
			{
				mType = static_cast<ParameterType>(mValue.which() + 1);
			}

			Parameter():mType(ParameterType::UNKNOWN)
			{

			}

			virtual ~Parameter() = default;
			
			std::string GetName()const
			{
				return mName;
			}
			ParameterType GetType()const
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

			template<typename ValueType>
			ValueType GetValue()const
			{
				return boost::get<ValueType>(mValue);
			}
		protected:
			using Variant = 
			//The order of template parameters must match that defined in ParameterType
			boost::variant
			<	
				float, 
				Foundation::Math::Vector2f, 
				Foundation::Math::Vector3f, 
				Foundation::Math::Vector4f, 
				Foundation::Math::Matrix4f, 
				std::shared_ptr<ITexture>, 
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
			ParameterType mType;
			std::string mName;
			Variant mValue;
		};
	}
}