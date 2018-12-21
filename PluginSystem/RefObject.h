#pragma once
#include <atomic>
#ifndef NDEBUG
#include <cassert>
#include <mutex>
#include <unordered_map>
#endif
#include "IRefObject.h"

#ifndef NDEBUG
#define NEW_REF_OBJ(Type, ...) new (__FILE__, #Type, __LINE__) Type(__VA_ARGS__)

#if defined(_MSC_VER)

#define FORCE_INLINE	__forceinline

#include <stdlib.h>

#define ROTL32(x,y)	_rotl(x,y)
#define ROTL64(x,y)	_rotl64(x,y)


// Other compilers

#else	// defined(_MSC_VER)

#define	FORCE_INLINE inline __attribute__((always_inline))

inline uint32_t rotl32 ( uint32_t x, int8_t r )
{
  return (x << r) | (x >> (32 - r));
}

inline uint64_t rotl64 ( uint64_t x, int8_t r )
{
  return (x << r) | (x >> (64 - r));
}

#define	ROTL32(x,y)	rotl32(x,y)
#define ROTL64(x,y)	rotl64(x,y)

#endif // !defined(_MSC_VER)

class RefMonitor
{
public:
	static RefMonitor* Instance()
	{
		static RefMonitor instance;
		return &instance;
	}

	RefMonitor::~RefMonitor()
	{
		if (!mAllocations.empty() || !mPointerInfos.empty())
		{
			assert(0);
		}
	}

	void* Allococate(std::size_t size, const char* file, const char* typeName, int line)
	{
		char temp[1024];
		auto len1 = std::strlen(file);
		auto len2 = std::strlen(typeName);
		auto len = len1 + len2 + sizeof(int);
		std::memcpy(temp, file, len1);
		std::memcpy(temp + len1, typeName, len2);
		assert(len <= 1024 && "Temp array not long enough!");
		*reinterpret_cast<int*>(&temp[len1 + len2]) = line;
		static uint32_t seed = 0x1082339f;
		uint32_t key;
		MurmurHash3_x86_32(temp, int(len), seed, &key);

		void* mem = std::malloc(size);
		{
			std::lock_guard<std::mutex> lock(mMapMutex);
			auto it = mAllocations.find(key);
			if (it == mAllocations.end())
			{
				Allocation allocation;
				allocation.file = file;
				allocation.typeName = typeName;
				allocation.line = line;
				allocation.size = size;
				allocation.count = 1;
				mAllocations[key] = allocation;
			}
			else
			{
				it->second.count += 1;
				it->second.size += size;
			}
			PointerInfo info;
			info.key = key;
			info.size = size;
			mPointerInfos[mem] = info;
		}
		return mem;
	}

	void Deallocate(void* p)
	{
		std::free(p);
		{
			std::lock_guard<std::mutex> lock(mMapMutex);
			auto& info = mPointerInfos[p];
			auto it = mAllocations.find(info.key);
			if (it != mAllocations.end())
			{
				it->second.size -= info.size;
				--it->second.count;
				if (it->second.count <= 0)
					mAllocations.erase(it);
			}
			mPointerInfos.erase(p);
		}
	}
private:
	struct Allocation
	{
		std::size_t size;
		const char* file;
		const char* typeName;
		int line;
		int count;
	};

	struct PointerInfo
	{
		uint32_t key;
		size_t size;
	};
	void MurmurHash3_x86_32 ( const void * key, int len, uint32_t seed, void * out )
	{
	  const uint8_t * data = (const uint8_t*)key;
	  const int nblocks = len / 4;

	  uint32_t h1 = seed;

	  const uint32_t c1 = 0xcc9e2d51;
	  const uint32_t c2 = 0x1b873593;

	  //----------
	  // body

	  const uint32_t * blocks = (const uint32_t *)(data + nblocks*4);

	  for(int i = -nblocks; i; i++)
	  {
		uint32_t k1 = getblock32(blocks,i);

		k1 *= c1;
		k1 = ROTL32(k1,15);
		k1 *= c2;
		
		h1 ^= k1;
		h1 = ROTL32(h1,13); 
		h1 = h1*5+0xe6546b64;
	  }

	  //----------
	  // tail

	  const uint8_t * tail = (const uint8_t*)(data + nblocks*4);

	  uint32_t k1 = 0;

	  switch(len & 3)
	  {
	  case 3: k1 ^= tail[2] << 16;
	  case 2: k1 ^= tail[1] << 8;
	  case 1: k1 ^= tail[0];
			  k1 *= c1; k1 = ROTL32(k1,15); k1 *= c2; h1 ^= k1;
	  };

	  //----------
	  // finalization

	  h1 ^= len;

	  h1 = fmix32(h1);

	  *(uint32_t*)out = h1;
	} 

	FORCE_INLINE uint32_t getblock32 ( const uint32_t * p, int i )
	{
	  return p[i];
	}

	FORCE_INLINE uint32_t fmix32 ( uint32_t h )
	{
	  h ^= h >> 16;
	  h *= 0x85ebca6b;
	  h ^= h >> 13;
	  h *= 0xc2b2ae35;
	  h ^= h >> 16;

	  return h;
	}

	std::unordered_map<uint32_t, Allocation> mAllocations;
	std::unordered_map<void*, PointerInfo> mPointerInfos;
	std::mutex mMapMutex;
};
#else
#define NEW_REF_OBJ(Type, ...) new Type(__VA_ARGS__)
#endif

#define REF_OBJECT_ADDREF_METHOD \
void AddRef()override\
{\
	mRefCount.fetch_add(1, std::memory_order_relaxed);\
}

#define REF_OBJECT_RELEASE_METHOD \
bool Release()override\
{\
	auto oldRefCountBase = mRefCount.fetch_sub(1, std::memory_order_relaxed);\
	if (oldRefCountBase == 1)\
	{\
		delete this;\
		return true;\
	}\
	return false;\
}

#define REF_OBJECT_GETREFCOUNT_METHOD \
int GetRefCount()const override{ return mRefCount; }

#ifndef NDEBUG
#define REF_OBJECT_NEW_OVERRIDE \
void* operator new(std::size_t size, const char* file, const char* typeName, int line)\
{\
	return RefMonitor::Instance()->Allococate(size, file, typeName, line);\
}\
void operator delete(void *p)\
{\
	RefMonitor::Instance()->Deallocate(p);\
}\
void operator delete(void *p, const char* file, const char* typeName, int line) \
{\
	RefMonitor::Instance()->Deallocate(p);\
}
#else
#define REF_OBJECT_NEW_OVERRIDE
#endif

#define REF_OBJECT_DISABLE_COPY(ClassName) \
ClassName(const ClassName&) = delete; \
ClassName& operator=(const ClassName&) = delete;


#define REF_OBJECT_METHODS

#define REF_OBJECT_OVERRIDE(ClassName) \
public:\
REF_OBJECT_ADDREF_METHOD \
REF_OBJECT_RELEASE_METHOD \
REF_OBJECT_GETREFCOUNT_METHOD \
REF_OBJECT_NEW_OVERRIDE \
REF_OBJECT_DISABLE_COPY(ClassName) \
private:\
std::atomic<int> mRefCount{1};



