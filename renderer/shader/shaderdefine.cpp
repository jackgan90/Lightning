#include <algorithm>
#include "shaderdefine.h"
#include "enginealgo.h"

namespace LightningGE
{
	namespace Renderer
	{
		ShaderDefine::ShaderDefine()
		{

		}

		ShaderDefine::ShaderDefine(const DefineMap& map)
		{
			m_define = map;
			Utility::erase_if(m_define, m_define.begin(), m_define.end(),
				[&](const std::pair<const std::string, const std::string>& macro) {return macro.first.length() <= 0; });
		}

		bool ShaderDefine::operator==(const ShaderDefine& define)const
		{
			if (m_define.size() != define.m_define.size())
				return false;
			for (auto it = m_define.begin(); it != m_define.end(); ++it)
			{
				auto it1 = define.m_define.find(it->first);
				if (it1 == define.m_define.end())
					return false;
				if (it->second != it1->second)
					return false;
			}
			return true;
		}

		ShaderDefine ShaderDefine::operator+(const ShaderDefine& define)const
		{
			ShaderDefine result(*this);
			result.Combine(define);

			return result;
		}

		ShaderDefine ShaderDefine::operator-(const ShaderDefine& define)const
		{
			ShaderDefine result(*this);
			result.Exclude(define);

			return result;
		}

		ShaderDefine& ShaderDefine::operator+=(const ShaderDefine& define)
		{
			Combine(define);
			return *this;
		}

		ShaderDefine& ShaderDefine::operator-=(const ShaderDefine& define)
		{
			Exclude(define);
			return *this;
		}


		void ShaderDefine::Combine(const ShaderDefine& define)
		{
			std::for_each(define.m_define.begin(), define.m_define.end(), 
				[&](const std::pair<std::string, std::string>& macro) {if(macro.first.length())m_define[macro.first] = macro.second; });
		}

		void ShaderDefine::Exclude(const ShaderDefine& define)
		{
			Utility::erase_if(m_define, m_define.begin(), m_define.end(),
				[&](const std::pair<const std::string, const std::string>& macro) {return define.HasMacro(macro.first); });
		}


		bool ShaderDefine::HasMacro(const std::string& macroName)const
		{
			return m_define.find(macroName) != m_define.end();
		}

		size_t ShaderDefine::GetMacroCount()const
		{
			return m_define.size();
		}


		const std::string ShaderDefine::GetMacros(const std::string& macroName)const
		{
			auto it = m_define.find(macroName);
			return it == m_define.end() ? "" : it->second;
		}

		void ShaderDefine::Define(const std::string& macroName, const std::string& macroValue)
		{
			if(macroName.length())
				m_define[macroName] = macroValue;
		}

		const DefineMap& ShaderDefine::GetAllDefine()const
		{
			return m_define;
		}
	}
}