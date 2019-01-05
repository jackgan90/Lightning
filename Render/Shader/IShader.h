#pragma once
#include <string>
#include <exception>
#include <boost/functional/hash.hpp>
#include "Semantics.h"
#include "RenderException.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"
#include "IRefObject.h"
#include "IShaderParameter.h"
#include "IShaderMacros.h"
#undef DOMAIN

namespace Lightning
{
	namespace Render
	{
		class ShaderCompileException : public RendererException
		{
		public:
			ShaderCompileException(const char*const w):RendererException(w){}
		};

		enum class ShaderType : std::uint8_t
		{
			VERTEX = 0,
			FRAGMENT,
			GEOMETRY,
			HULL, //hull shader
			DOMAIN,	//domain shader
			SHADER_TYPE_NUM
		};

		struct IShader :  Plugins::IRefObject
		{
			virtual ShaderType INTERFACECALL GetType()const = 0;
			virtual void INTERFACECALL DefineMacros(const IShaderMacros* macros) = 0;
			virtual const IShaderMacros* INTERFACECALL GetMacros()const = 0;
			virtual std::size_t INTERFACECALL GetParameterCount()const = 0;
			virtual void INTERFACECALL Compile() = 0;
			virtual const char* INTERFACECALL GetName()const = 0;
			virtual bool INTERFACECALL SetParameter(const IShaderParameter* parameter) = 0;
			virtual const char* const INTERFACECALL GetSource()const = 0;
			virtual void INTERFACECALL GetUniformSemantics(RenderSemantics** semantics, std::uint16_t& semanticCount) = 0;
			virtual std::size_t INTERFACECALL GetHash()const = 0;
		};
	}
}

