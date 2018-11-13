#include "Common.h"
#include "MurmurHash3.h"

namespace Lightning
{
	namespace Foundation
	{
		std::uint32_t Hash(const void* key, std::size_t len, std::uint32_t seed)
		{
			std::uint32_t out;
			MurmurHash3_x86_32(key, static_cast<int>(len), seed, &out);
			return out;
		}

		void Hash(const void* key, std::size_t len, std::uint32_t seed, void* out)
		{
			MurmurHash3_x86_128(key, static_cast<int>(len), seed, out);
		}
	}
}