#pragma once
#include <cstring>
#include <cstdint>
#include <random>
#include <type_traits>
#include <functional>
#include "common.h"

#define PLAIN_OBJECT_HASH_SPECILIZATION(Class)\
namespace std{\
	template<> struct std::hash<Class>\
	{\
		std::size_t operator()(const Class& state)const noexcept\
		{\
			return state.GetHash();\
		}\
	};\
}\

namespace Lightning
{
	namespace Foundation
	{
		template<typename Derived>
		struct PlainObject
		{
			bool operator==(const Derived& other)const
			{
				return std::memcmp(this, &other, sizeof(other)) == 0;
			}

			bool operator!=(const Derived& other)const
			{
				return !this->operator==(other);
			}

			PlainObject& operator=(const Derived& other)
			{
				std::memcpy(this, &other, sizeof(other));
				return *this;
			}

			std::uint32_t GetHash()const
			{
				static std::random_device rd;
				static std::mt19937 mt(rd());
				static std::uniform_int_distribution<std::uint32_t> dist(1, static_cast<std::uint32_t>(-1));
				static std::uint32_t seed = dist(mt);
				return Hash(this, sizeof(Derived), seed);
			}
		};
	}
}
