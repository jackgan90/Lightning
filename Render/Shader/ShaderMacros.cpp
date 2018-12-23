#include <algorithm>
#include <boost/functional/hash.hpp>
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


		bool ShaderMacros::HasMacro(const char* macroName)const
		{
			return mMacros.find(macroName) != mMacros.end();
		}

		size_t ShaderMacros::GetMacroCount()const
		{
			return mMacros.size();
		}


		const char* ShaderMacros::GetMacroValue(const char* macroName)const
		{
			auto it = mMacros.find(macroName);
			return it == mMacros.end() ? "" : it->second.c_str();
		}

		void ShaderMacros::Define(const char* macroName, const char* macroValue)
		{
			if(macroName)
				mMacros[macroName] = macroValue;
		}

		void ShaderMacros::Undefine(const char* macroName)
		{
			mMacros.erase(macroName);
		}

		void ShaderMacros::GetAllMacros(MacroPair** pairs)const
		{
			auto macroArray = *pairs;
			auto i = 0;
			for (auto it = mMacros.cbegin(); it != mMacros.cend();++it, ++i)
			{
				macroArray[i].name = it->first.c_str();
				macroArray[i].value = it->second.c_str();
			}
		}

		std::size_t ShaderMacros::GetHash()const
		{
			std::size_t seed = 0x43d6799;
			for (auto it = mMacros.begin(); it != mMacros.end(); ++it)
			{
				boost::hash_combine(seed, it->first);
				boost::hash_combine(seed, it->second);
			}
			return seed;
		}

		const char* ShaderMacros::GetMacroString()const
		{
			static std::string str;
			str.clear();
			for (auto it = mMacros.cbegin(); it != mMacros.cend();++it)
			{
				str += it->first + " " + it->second + '\n';
			}

			return str.c_str();
		}
	}
}