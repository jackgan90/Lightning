#include <algorithm>
#include "shaderdefine.h"
#include "enginealgo.h"

namespace Lightning
{
	namespace Render
	{
		ShaderDefine::ShaderDefine()
		{

		}

		ShaderDefine::ShaderDefine(const MacroContainer& map)
		{
			mMacros = map;
			Utility::erase_if(mMacros, mMacros.begin(), mMacros.end(),
				[&](const std::pair<const std::string, const std::string>& macro) {return macro.first.length() <= 0; });
		}

		bool ShaderDefine::operator==(const ShaderDefine& define)const
		{
			if (mMacros.size() != define.mMacros.size())
				return false;
			for (auto it = mMacros.begin(); it != mMacros.end(); ++it)
			{
				auto it1 = define.mMacros.find(it->first);
				if (it1 == define.mMacros.end())
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
			std::for_each(define.mMacros.begin(), define.mMacros.end(), 
				[&](const std::pair<std::string, std::string>& macro) {if(macro.first.length())mMacros[macro.first] = macro.second; });
		}

		void ShaderDefine::Exclude(const ShaderDefine& define)
		{
			Utility::erase_if(mMacros, mMacros.begin(), mMacros.end(),
				[&](const std::pair<const std::string, const std::string>& macro) {return define.HasMacro(macro.first); });
		}


		bool ShaderDefine::HasMacro(const std::string& macroName)const
		{
			return mMacros.find(macroName) != mMacros.end();
		}

		size_t ShaderDefine::GetMacroCount()const
		{
			return mMacros.size();
		}


		const std::string ShaderDefine::GetMacroValue(const std::string& macroName)const
		{
			auto it = mMacros.find(macroName);
			return it == mMacros.end() ? "" : it->second;
		}

		void ShaderDefine::Define(const std::string& macroName, const std::string& macroValue)
		{
			if(macroName.length())
				mMacros[macroName] = macroValue;
		}

		const MacroContainer& ShaderDefine::GetAllMacros()const
		{
			return mMacros;
		}
	}
}