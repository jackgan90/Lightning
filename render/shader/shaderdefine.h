#pragma once
#include <memory>
#include <unordered_map>
#include <string>
#include "rendererexportdef.h"


namespace Lightning
{
	namespace Render
	{
		using DefineMap = std::unordered_map<std::string, std::string>;
		class LIGHTNING_RENDER_API ShaderDefine
		{
		public:
			ShaderDefine();
			ShaderDefine(const DefineMap& map);
			bool operator==(const ShaderDefine& define)const;
			ShaderDefine operator+(const ShaderDefine& define)const;
			ShaderDefine operator-(const ShaderDefine& define)const;
			ShaderDefine& operator+=(const ShaderDefine& define);
			ShaderDefine& operator-=(const ShaderDefine& define);
			void Combine(const ShaderDefine& define);
			void Exclude(const ShaderDefine& define);
			bool HasMacro(const std::string& macroName)const;
			void Define(const std::string& macroName, const std::string& macroValue);
			const std::string GetMacros(const std::string& macroName)const;
			const DefineMap& GetAllDefine()const;
			size_t GetMacroCount()const;
		private:
			DefineMap m_define;
		};
	}
}
