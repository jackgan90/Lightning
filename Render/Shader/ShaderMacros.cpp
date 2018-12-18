#include <algorithm>
#include "ShaderMacros.h"

namespace Lightning
{
	namespace Render
	{
		ShaderMacros::ShaderMacros()
		{

		}

		ShaderMacros::ShaderMacros(const MacroContainer& map)
		{
			mMacros = map;
		}

		bool ShaderMacros::operator==(const ShaderMacros& define)const
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

		ShaderMacros ShaderMacros::operator+(const ShaderMacros& define)const
		{
			ShaderMacros result(*this);
			result.Combine(define);

			return result;
		}

		ShaderMacros ShaderMacros::operator-(const ShaderMacros& define)const
		{
			ShaderMacros result(*this);
			result.Exclude(define);

			return result;
		}

		ShaderMacros& ShaderMacros::operator+=(const ShaderMacros& define)
		{
			Combine(define);
			return *this;
		}

		ShaderMacros& ShaderMacros::operator-=(const ShaderMacros& define)
		{
			Exclude(define);
			return *this;
		}


		void ShaderMacros::Combine(const ShaderMacros& shaderMacros)
		{
			std::for_each(shaderMacros.mMacros.begin(), shaderMacros.mMacros.end(), 
				[&](const std::pair<std::string, std::string>& macro) {if(macro.first.length())mMacros[macro.first] = macro.second; });
		}

		void ShaderMacros::Exclude(const ShaderMacros& shaderMacros)
		{
		}


		bool ShaderMacros::HasMacro(const std::string& macroName)const
		{
			return mMacros.find(macroName) != mMacros.end();
		}

		size_t ShaderMacros::GetMacroCount()const
		{
			return mMacros.size();
		}


		const std::string ShaderMacros::GetMacroValue(const std::string& macroName)const
		{
			auto it = mMacros.find(macroName);
			return it == mMacros.end() ? "" : it->second;
		}

		void ShaderMacros::Define(const std::string& macroName, const std::string& macroValue)
		{
			if(macroName.length())
				mMacros[macroName] = macroValue;
		}

		const MacroContainer& ShaderMacros::GetAllMacros()const
		{
			return mMacros;
		}

		std::string ShaderMacros::GetMacroString()const
		{
			std::string str;
			for (auto it = mMacros.cbegin(); it != mMacros.cend();++it)
			{
				str += it->first + " " + it->second + '\n';
			}

			return str;
		}
	}
}