#pragma once
#include <memory>
#include <string>
#include "container.h"
#include "rendererexportdef.h"


namespace Lightning
{
	namespace Render
	{
		using Foundation::container;
		using MacroContainer = container::unordered_map<std::string, std::string>;
		class LIGHTNING_RENDER_API ShaderDefine
		{
		public:
			ShaderDefine();
			ShaderDefine(const MacroContainer& map);
			bool operator==(const ShaderDefine& define)const;
			ShaderDefine operator+(const ShaderDefine& define)const;
			ShaderDefine operator-(const ShaderDefine& define)const;
			ShaderDefine& operator+=(const ShaderDefine& define);
			ShaderDefine& operator-=(const ShaderDefine& define);
			void Combine(const ShaderDefine& define);
			void Exclude(const ShaderDefine& define);
			bool HasMacro(const std::string& macroName)const;
			void Define(const std::string& macroName, const std::string& macroValue);
			const std::string GetMacroValue(const std::string& macroName)const;
			const MacroContainer& GetAllMacros()const;
			size_t GetMacroCount()const;
		private:
			MacroContainer mMacros;
		};
	}
}
