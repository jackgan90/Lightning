#pragma once
#include <string>
#include "Container.h"
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
			virtual INTERFACECALL ~IShaderMacros() = default;
			virtual bool INTERFACECALL HasMacro(const char* macroName)const = 0;
			virtual void INTERFACECALL Define(const char* macroName, const char* macroValue) = 0;
			virtual void INTERFACECALL Undefine(const char* macroName) = 0;
			virtual const char* INTERFACECALL GetMacroValue(const char*  macroName)const = 0;
			virtual std::size_t INTERFACECALL GetMacroCount()const = 0;
			virtual const char* INTERFACECALL GetMacroString()const = 0;
			//Get all macros.Caller must ensure the address pointed to by *pairs has enough memory to 
			//store all of the macros.(Possibly by call GetMacroCount and pre-allocate enough memory)
			virtual void INTERFACECALL GetAllMacros(MacroPair** pairs)const = 0;
			virtual std::size_t INTERFACECALL GetHash()const = 0;
		};

		using Foundation::Container;
		using MacroContainer = Container::UnorderedMap<std::string, std::string>;
		class ShaderMacros : public IShaderMacros
		{
		public:
			ShaderMacros();
			ShaderMacros(const MacroContainer& map);
			bool operator==(const ShaderMacros& shaderMacros)const;
			ShaderMacros operator+(const ShaderMacros& shaderMacros)const;
			ShaderMacros operator-(const ShaderMacros& shaderMacros)const;
			ShaderMacros& operator+=(const ShaderMacros& shaderMacros);
			ShaderMacros& operator-=(const ShaderMacros& shaderMacros);
			void Combine(const ShaderMacros& shaderMacros);
			void Exclude(const ShaderMacros& shaderMacros);
			bool INTERFACECALL HasMacro(const char* macroName)const override;
			void INTERFACECALL Define(const char* macroName, const char* macroValue)override;
			void INTERFACECALL Undefine(const char* macroName)override;
			const char* INTERFACECALL GetMacroValue(const char* macroName)const override;
			size_t INTERFACECALL GetMacroCount()const override;
			const char* INTERFACECALL GetMacroString()const override;
			void INTERFACECALL GetAllMacros(MacroPair** pairs)const override;
			std::size_t INTERFACECALL GetHash()const override;
		private:
			MacroContainer mMacros;
		};
	}
}
