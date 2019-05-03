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
			virtual void DefineMacros(const IShaderMacros* macros) = 0;
			virtual const IShaderMacros* GetMacros()const = 0;
			virtual std::size_t GetParameterCount()const = 0;
			virtual void Compile() = 0;
			virtual const char* GetName()const = 0;
			virtual bool SetParameter(const IShaderParameter* parameter) = 0;
			virtual const char* const GetSource()const = 0;
			virtual void GetUniformSemantics(RenderSemantics** semantics, std::uint16_t& semanticCount) = 0;
			virtual std::size_t GetHash()const = 0;
		};
	}
}

