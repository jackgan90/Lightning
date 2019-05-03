#pragma once
#include <cstddef>
#include "Portable.h"

namespace Lightning
{
	namespace Render
	{
		struct MacroPair
		{
			const char* name;
			const char* value;
		};

		struct IShaderMacros
		{
			virtual ~IShaderMacros() = default;
			virtual bool HasMacro(const char* macroName)const = 0;
			virtual void Define(const char* macroName, const char* macroValue) = 0;
			virtual void Undefine(const char* macroName) = 0;
			virtual const char* GetMacroValue(const char*  macroName)const = 0;
			virtual std::size_t GetMacroCount()const = 0;
			virtual const char* GetMacroString()const = 0;
			//Get all macros.Caller must ensure the address pointed to by *pairs has enough memory to 
			//store all of the macros.(Possibly by call GetMacroCount and pre-allocate enough memory)
			virtual void GetAllMacros(MacroPair** pairs)const = 0;
			virtual std::size_t GetHash()const = 0;
		};

	}
}