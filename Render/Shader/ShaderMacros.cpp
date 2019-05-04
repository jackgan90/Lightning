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


		bool ShaderMacros::IsDefined(const std::string& macroName)const
		{
			return mMacros.find(macroName) != mMacros.end();
		}

		size_t ShaderMacros::GetMacroCount()const
		{
			return mMacros.size();
		}


		bool ShaderMacros::GetMacroValue(const std::string& macroName, std::string& macroValue)const
		{
			auto it = mMacros.find(macroName);
			if (it == mMacros.end())
			{
				return false;
			}
			else
			{
				macroValue = it->second;
				return true;
			}
		}

		void ShaderMacros::Define(const std::string& macroName, const std::string& macroValue)
		{
			assert(!macroName.empty() && "Defined macro name can't be empty!");
			mMacros[macroName] = macroValue;
		}

		void ShaderMacros::Undefine(const std::string& macroName)
		{
			mMacros.erase(macroName);
		}

		void ShaderMacros::GetAllMacros(std::vector<std::pair<std::string, std::string>>& macros)const
		{
			for (auto it = mMacros.cbegin(); it != mMacros.cend();++it)
			{
				macros.emplace_back(std::make_pair(it->first, it->second));
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

		std::string ShaderMacros::GetMacroString()const
		{
			std::string macroString;
			for (auto it = mMacros.cbegin(); it != mMacros.cend();++it)
			{
				macroString += it->first + " " + it->second + "\n";
			}

			return macroString;
		}
	}
}