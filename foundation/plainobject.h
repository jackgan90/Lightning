#pragma once
#include <cstring>
#include <cstdint>
#include <random>
#include <type_traits>
#include "murmurhash3.h"

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

			std::size_t GetHash()const
			{
				static std::random_device rd;
				static std::mt19937 mt(rd());
				static std::uniform_int_distribution<std::uint32_t> dist(1, static_cast<std::uint32_t>(-1));
				static std::uint32_t seed = dist(mt);
				std::size_t out;
				MurmurHash3_x86_32(this, sizeof(Derived), seed, &out);
				return out;
			}
		};
	}
}
