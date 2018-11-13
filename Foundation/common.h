#pragma once
#include <cstdint>
#include <cstdlib> //for size_t
#include "foundationexportdef.h"

#define DYNAMIC_CAST_PTR(subclass, smartptr) (dynamic_cast<subclass*>(smartptr.get()))
#define STATIC_CAST_PTR(subclass, smartptr) (static_cast<subclass*>(smartptr.get()))
#define SAFE_DELETE(p) do{if((p)){delete (p); (p) = nullptr;}}while(0)
#define SAFE_RELEASE(p) do{ if((p)){(p)->Release(); (p) = 0;}}while(0)

namespace Lightning
{
	namespace Foundation
	{
		LIGHTNING_FOUNDATION_API std::uint32_t Hash(const void* key, std::size_t len, std::uint32_t seed);
		LIGHTNING_FOUNDATION_API void Hash(const void* key, std::size_t len, std::uint32_t seed, void* out);
	}
}
