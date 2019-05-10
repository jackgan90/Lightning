#pragma once
#include <string>
#include <exception>
#include <boost/functional/hash.hpp>
#include "Semantics.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"
#include "IShaderParameter.h"
#include "IShaderMacros.h"
#undef DOMAIN

namespace Lightning
{
	namespace Render
	{
		enum class ShaderType : std::uint8_t
		{
			VERTEX = 1,
			FRAGMENT,
			GEOMETRY,
			HULL, //hull shader
			DOMAIN,	//domain shader
		};

		struct IShader
		{
			virtual ~IShader() = default;
			virtual ShaderType GetType()const = 0;
			virtual void DefineMacro(const std::string& macroName, const std::string& macroValue) = 0;
			virtual std::shared_ptr<IShaderMacros> GetMacros()const = 0;
			virtual std::size_t GetParameterCount()const = 0;
			virtual void Compile() = 0;
			virtual std::string GetName()const = 0;
			virtual bool SetParameter(const IShaderParameter& parameter) = 0;
			virtual ShaderParameterType GetParameterType(const std::string& name)const = 0;
			virtual std::string GetSource()const = 0;
			virtual void GetUniformSemantics(RenderSemantics** semantics, std::uint16_t& semanticCount) = 0;
			virtual std::size_t GetHash()const = 0;
		};
	}
}

