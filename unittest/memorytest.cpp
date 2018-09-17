#include <cstdlib>
#include <vector>
#include <random>
#include <array>
#include <sstream>
#include <iostream>
#include <chrono>
#include <unordered_map>
#include <cstdint>
#include "catch.hpp"
#include "stackallocator.h"
#include "poolallocator.h"
#include "ringallocator.h"

using Lightning::Foundation::IMemoryAllocator;
using Lightning::Foundation::PoolAllocator;
using Lightning::Foundation::StackAllocator;
using Lightning::Foundation::RingAllocator;

namespace 
{
	/*
	template<bool AlignedAlloc, const size_t Alignment, const size_t BlockSize>
	void TestStackAllocatorAlignment(StackAllocator<AlignedAlloc, Alignment, BlockSize>& allocator)
	{
		if (AlignedAlloc)
		{
			void* p = ALLOC(&allocator, 100, void);
			REQUIRE(reinterpret_cast<size_t>(p) % Alignment == 0);
			DEALLOC(&allocator, p);
		}
	}

	template<bool AlignedAlloc, const size_t Alignment, const size_t BlockSize>
	void TestStackAllocatorSingleAlloc(StackAllocator<AlignedAlloc, Alignment, BlockSize>& allocator)
	{
		char* mem = ALLOC(&allocator, 3000, char);
		REQUIRE(allocator.GetAllocatedSize() == 3000);
		REQUIRE(allocator.GetAllocatedCount() == 1);
		DEALLOC(&allocator, mem);
		REQUIRE(allocator.GetAllocatedSize() == 0);
		REQUIRE(allocator.GetAllocatedCount() == 0);

		struct TestStruct 
		{
			std::uint16_t field0;
			std::uint8_t field1;
			std::uint32_t field2;
		};
		TestStruct* ts = ALLOC_ARRAY(&allocator, 100, TestStruct);
		REQUIRE(allocator.GetAllocatedSize() == sizeof(TestStruct) * 100);
		DEALLOC(&allocator, ts);
		REQUIRE(allocator.GetAllocatedCount() == 0);
		REQUIRE(allocator.GetAllocatedSize() == 0);
	}

	template<bool AlignedAlloc, const size_t Alignment, const size_t BlockSize>
	void TestStackAllocatorAllocOrder(StackAllocator<AlignedAlloc, Alignment, BlockSize>& allocator)
	{
		int* mem0 = ALLOC(&allocator, sizeof(int) * 200, int);
		char* mem1 = ALLOC(&allocator, sizeof(char) * 150, char);
		DEALLOC(&allocator, mem0);
		REQUIRE(allocator.GetAllocatedCount() == 2 );
		REQUIRE(allocator.GetAllocatedSize() == (sizeof(int) * 200 + sizeof(char) * 150));
		DEALLOC(&allocator, mem1);
		REQUIRE(allocator.GetAllocatedCount() == 0 );
		REQUIRE(allocator.GetAllocatedSize() == 0);

		std::default_random_engine engine;
		std::uniform_int_distribution<int> dist;
		std::vector<void*> allocMem;
		for (unsigned int i = 0; i < 100; i++)
			allocMem.push_back(ALLOC(&allocator, dist(engine) % 1000, void));
		auto allocatedSize = allocator.GetAllocatedSize();
		auto allocatedCount = allocator.GetAllocatedCount();
		auto usedBlockCount = allocator.GetNonEmptyBlockCount();
		for (unsigned int i = 0; i < allocMem.size(); i++)
		{
			DEALLOC(&allocator, allocMem[i]);
			auto blockCount = allocator.GetNonEmptyBlockCount();
			if (blockCount == usedBlockCount)
			{
				CAPTURE(AlignedAlloc);
				CAPTURE(Alignment);
				CAPTURE(BlockSize);
				CAPTURE(blockCount);
				REQUIRE(allocator.GetAllocatedSize() == allocatedSize);
				REQUIRE(allocator.GetAllocatedCount() == allocatedCount);
			}
			else
			{
				usedBlockCount = blockCount;
				allocatedCount = allocator.GetAllocatedCount();
				allocatedSize = allocator.GetAllocatedSize();
			}
		}
		allocMem.clear();
		REQUIRE(allocator.GetAllocatedSize() == 0);
		REQUIRE(allocator.GetAllocatedCount() == 0);

		for (auto i = 0; i < 100; i++)
			allocMem.push_back(ALLOC(&allocator, dist(engine) % 1000 + 1, void));

		std::random_device rd;
		std::mt19937 g(rd());
		std::shuffle(allocMem.begin(), allocMem.end(), g);
		for (unsigned int i = 0; i < allocMem.size(); i++)
			DEALLOC(&allocator, allocMem[i]);
		REQUIRE(allocator.GetAllocatedSize() == 0);
		REQUIRE(allocator.GetAllocatedCount() == 0);
		allocMem.clear();

		//alloc then dealloc , alloc then dealloc
		for (unsigned int i = 0; i < 100; ++i)
			allocMem.push_back(ALLOC(&allocator, dist(engine) % 333, void));

		for (unsigned int i = 0; i < 50; ++i)
		{
			DEALLOC(&allocator, allocMem[i]);
			allocMem[i] = ALLOC(&allocator, dist(engine) % 666, void);
		}

		for (unsigned int i = 0; i < 50; ++i)
		{
			DEALLOC(&allocator, allocMem[i]);
		}

		for (unsigned int i = 0; i < 50; ++i)
		{
			allocMem[i] = ALLOC(&allocator, dist(engine) % 444, void);
		}

		for (size_t i = 0; i < allocMem.size(); ++i)
		{
			DEALLOC(&allocator, allocMem[i]);
		}
		REQUIRE(allocator.GetAllocatedSize() == 0);
		REQUIRE(allocator.GetAllocatedCount() == 0);
		allocMem.clear();
	}

	template<bool AlignedAlloc, const size_t Alignment, const size_t BlockSize>
	void TestStackAllocatorNewBlock(StackAllocator<AlignedAlloc, Alignment, BlockSize>& allocator)
	{
		auto blockSize = BlockSize;
		char* mem0 = ALLOC(&allocator, static_cast<size_t>(2.0 * blockSize / 3), char);
		REQUIRE(allocator.GetNonEmptyBlockCount() == 1);
		char* mem1 = ALLOC(&allocator, static_cast<size_t>(blockSize / 2), char);
		REQUIRE(allocator.GetNonEmptyBlockCount() == 2);
		DEALLOC(&allocator, mem0);
		REQUIRE(allocator.GetNonEmptyBlockCount() == 1);
		DEALLOC(&allocator, mem1);
		REQUIRE(allocator.GetNonEmptyBlockCount() == 0);
	}

	template<bool AlignedAlloc, const size_t Alignment, const size_t BlockSize>
	void TestStackAllocatorOverflow(StackAllocator<AlignedAlloc, Alignment, BlockSize>& allocator)
	{
		int* mem = ALLOC_ARRAY(&allocator, 100, int);
		mem[99] = 9999999;
		mem[0] = 9999999;
		DEALLOC(&allocator, mem);
		std::vector<int*> mems;
		std::default_random_engine engine;
		std::uniform_int_distribution<int> dist;
		for (unsigned int j = 0; j < 100; j++)
		{
			size_t arr_size = dist(engine) % 500 + 1;
			mems.push_back(ALLOC_ARRAY(&allocator, arr_size, int));
			std::memset(mems[mems.size() - 1], 0, sizeof(int) * arr_size);
		}
		for (auto it = mems.cbegin(); it != mems.cend(); ++it)
		{
			DEALLOC(&allocator, *it);
		}
		mems.clear();
		REQUIRE(allocator.GetAllocatedSize() == 0);
		REQUIRE(allocator.GetAllocatedCount() == 0);
	}

	template<bool AlignedAlloc, const size_t Alignment, const size_t BlockSize>
	void TestStackAllocator(StackAllocator<AlignedAlloc, Alignment, BlockSize>& allocator)
	{
		std::stringstream ss;
		ss << AlignedAlloc << Alignment << BlockSize;
		SECTION("Alignment test" + ss.str())
		{
			TestStackAllocatorAlignment(allocator);
		}
		SECTION("Single allocate and deallocate" + ss.str()) 
		{
			TestStackAllocatorSingleAlloc(allocator);
		}
		SECTION("Deallocation order" + ss.str()) 
		{
			TestStackAllocatorAllocOrder(allocator);
		}
		SECTION("New block allocation test" + ss.str()) 
		{
			TestStackAllocatorNewBlock(allocator);
		}
		SECTION("StackAllocator overflow test" + ss.str()) 
		{
			TestStackAllocatorOverflow(allocator);
		}
	}
	TEST_CASE("Stack allocator functional test.", "[StackAllocator function]") 
	{
		StackAllocator<true, 16, 4096> allocator0;
		StackAllocator<true, 128, 4096> allocator1;
		StackAllocator<true, 16, 8192> allocator2;
		StackAllocator<false, 16, 4096> allocator3;
		StackAllocator<false, 128, 4096> allocator4;
		StackAllocator<false, 16, 65536> allocator5;

		TestStackAllocator(allocator0);
		TestStackAllocator(allocator1);
		TestStackAllocator(allocator2);
		TestStackAllocator(allocator3);
		TestStackAllocator(allocator4);
		TestStackAllocator(allocator5);
	}

	template<bool AlignedAlloc, const size_t Alignment, const size_t BlockSize>
	void TestStackAllocatorPerformance(StackAllocator<AlignedAlloc, Alignment, BlockSize>& allocator)
	{
		using std::chrono::duration;
		using std::chrono::duration_cast;
		constexpr const int AllocateCount = 30000;
		constexpr const int AllocateUnit = 100;
		std::stringstream ss;
		ss << AlignedAlloc << Alignment << BlockSize;
		std::uint8_t* memArray[AllocateCount];
		SECTION("PerfTest" + ss.str())
		{
			std::cout << "alignAlloc:" << std::boolalpha << AlignedAlloc << ",alignment:" << Alignment << ",blockSize:" << BlockSize << std::endl;
			auto malloc_start = std::chrono::high_resolution_clock::now();
			for (auto i = 0; i < AllocateCount; ++i)
				memArray[i] = reinterpret_cast<std::uint8_t*>(std::malloc(AllocateUnit));
			auto malloc_end = std::chrono::high_resolution_clock::now();

			std::cout << "[std::malloc allocation time:] " << duration_cast<duration<double>>(malloc_end - malloc_start).count() << std::endl;

			auto free_start = std::chrono::high_resolution_clock::now();
			for (auto i = 0; i < AllocateCount; ++i)
				std::free(memArray[i]);
			auto free_end = std::chrono::high_resolution_clock::now();

			std::cout << "[std::free deallocation time:] " << duration_cast<duration<double>>(free_end - free_start).count() << std::endl;

			auto new_start = std::chrono::high_resolution_clock::now();
			for (auto i = 0; i < AllocateCount; ++i)
				memArray[i] = new std::uint8_t[AllocateUnit];
			auto new_end = std::chrono::high_resolution_clock::now();

			std::cout << "[new allocation time:] " << duration_cast<duration<double>>(new_end - new_start).count() << std::endl;

			auto delete_start = std::chrono::high_resolution_clock::now();
			for (auto i = 0; i < AllocateCount; ++i)
				delete[] memArray[i];
			auto delete_end = std::chrono::high_resolution_clock::now();
			std::cout << "[delete deallocation time:] " << duration_cast<duration<double>>(delete_end - delete_start).count() << std::endl;

			auto allocator_alloc_start = std::chrono::high_resolution_clock::now();
			for (auto i = 0; i < AllocateCount; ++i)
				memArray[i] = ALLOC(&allocator, AllocateUnit, std::uint8_t);
			auto allocator_alloc_end = std::chrono::high_resolution_clock::now();

			std::cout << "[allocator allocation time:] " << duration_cast<duration<double>>(allocator_alloc_end - allocator_alloc_start).count() << std::endl;
			//std::cout << "Allocator block count: " << allocator.GetBlockCount() << std::endl;

			auto allocator_dealloc_start = std::chrono::high_resolution_clock::now();
			for (auto i = 0; i < AllocateCount; ++i)
				DEALLOC(&allocator, memArray[i]);
			auto allocator_dealloc_end = std::chrono::high_resolution_clock::now();
			REQUIRE(allocator.GetAllocatedCount() == 0);
			REQUIRE(allocator.GetAllocatedSize() == 0);

			std::cout << "[allocator deallocation time:] " << duration_cast<duration<double>>(allocator_dealloc_end - allocator_dealloc_start).count() << std::endl;

			std::cout << "====================StackAllocator performance test end==========================" << std::endl;
		}

	}

	
	TEST_CASE("Stack allocator performance test", "[StackAllocator performance]") 
	{
		StackAllocator<true, 16, 4096> allocator0;
		StackAllocator<true, 128, 4096> allocator1;
		StackAllocator<true, 16, 8192> allocator2;
		StackAllocator<false, 16, 4096> allocator3;
		StackAllocator<false, 128, 4096> allocator4;
		StackAllocator<false, 16, 65536> allocator5;

		TestStackAllocatorPerformance(allocator0);
		TestStackAllocatorPerformance(allocator1);
		TestStackAllocatorPerformance(allocator2);
		TestStackAllocatorPerformance(allocator3);
		TestStackAllocatorPerformance(allocator4);
		TestStackAllocatorPerformance(allocator5);
	}
	

	struct PoolTestObject
	{
		PoolTestObject() :a(0), b(0), c(0.0f), d(0.0) {}
		PoolTestObject(const int a, const char b)
		{
			this->a = a;
			this->b = b;
		}
		int a;
		char b;
		float c;
		double d;
		char e[10];
		PoolTestObject& operator=(const int param)
		{
			a = 0;
			b = 0;
			c = 0.0f;
			d = 0.0;
			std::memset(e, 0, sizeof(e));
			return *this;
		}
	};

	struct PoolObjOfSize16
	{
		int a;
		int b;
		int c;
		int d;
		PoolObjOfSize16& operator=(const int param) { return *this; }
	};

	template<typename T, const size_t ObjectCount, bool AlignAlloc, const size_t Alignment>
	void TestPoolAllocator(PoolAllocator<T, ObjectCount, AlignAlloc, Alignment>& allocator)
	{
		static_assert(!AlignAlloc || Alignment > 0, "Alignment must be greater than 0!");
		std::vector<T*> mems;
		for (size_t i = 0; i < ObjectCount+1; ++i)
		{
			auto p = allocator.GetObject();
			if (AlignAlloc)
			{
				REQUIRE(reinterpret_cast<size_t>(p) % Alignment == 0);
			}
			if (p)
			{
				*p = 0;
			}
			mems.push_back(p);
			if (i < ObjectCount)
			{
				CAPTURE(i);
				REQUIRE(mems.back() != nullptr);
			}
			else
			{
				REQUIRE(mems.back() == nullptr);
			}
		}
		for (size_t i = 0; i < ObjectCount; ++i)
			allocator.ReleaseObject(mems[i]);
		mems.clear();

		for (size_t i = 0; i < ObjectCount; ++i)
		{
			auto p = allocator.GetObject();
			*p = 0;
			mems.push_back(p);
		}
		REQUIRE(allocator.GetAllocatedSize() == ObjectCount * sizeof(T));
		std::random_device rd;
		std::mt19937 g(rd());
		std::shuffle(mems.begin(), mems.end(), g);
		for (size_t i = 0; i < ObjectCount; ++i)
			allocator.ReleaseObject(mems[i]);

		REQUIRE(allocator.GetAllocatedCount() == 0);
		REQUIRE(allocator.GetAllocatedSize() == 0);
	}

	TEST_CASE("PoolAllocator size test") 
	{
		PoolAllocator<char, 100, false, 0> char_100_unaligned_allocator;
		PoolAllocator<char, 100, true, 16> char_100_aligned_16_allocator;
		PoolAllocator<char, 100, true, 64> char_100_aligned_64_allocator;
		char* a = char_100_aligned_16_allocator.GetObject('c');
		REQUIRE(reinterpret_cast<size_t>(a) % 16 == 0);
		REQUIRE(*a == 'c');
		char_100_aligned_16_allocator.ReleaseObject(a);
		a = char_100_unaligned_allocator.GetObject();
		REQUIRE(a != nullptr);
		REQUIRE(char_100_unaligned_allocator.GetAllocatedCount() == 1);
		REQUIRE(char_100_unaligned_allocator.GetAllocatedSize() == sizeof(char));
		char_100_unaligned_allocator.ReleaseObject(a);
		REQUIRE(char_100_unaligned_allocator.GetAllocatedCount() == 0);
		REQUIRE(char_100_unaligned_allocator.GetAllocatedSize() == 0);

		TestPoolAllocator(char_100_unaligned_allocator);
		TestPoolAllocator(char_100_aligned_16_allocator);
		TestPoolAllocator(char_100_aligned_64_allocator);

		PoolAllocator<PoolTestObject, 900, false, 0> obj_900_unaligned_allocator;
		PoolAllocator<PoolTestObject, 900, true, 128> obj_900_aligned_128_allocator;
		PoolTestObject* obj = obj_900_unaligned_allocator.GetObject();
		REQUIRE(obj != nullptr);
		REQUIRE(obj->a == 0);
		REQUIRE(obj->b == 0);
		REQUIRE(obj->c == 0.0f);
		REQUIRE(obj->d == 0.0);
		obj_900_unaligned_allocator.ReleaseObject(obj);
		TestPoolAllocator(obj_900_unaligned_allocator);
		TestPoolAllocator(obj_900_aligned_128_allocator);

		PoolAllocator<PoolObjOfSize16, 100, true, sizeof(PoolObjOfSize16)> obj_100_aligned_specified_allocator;
		TestPoolAllocator(obj_100_aligned_specified_allocator);
	}

	TEST_CASE("Frame Ring Allocator Test")
	{
		RingAllocator allocator;
		std::size_t usedSize{ 0 };
		auto p = allocator.Allocate<std::uint8_t>(10);
		REQUIRE(p != nullptr);
		REQUIRE(allocator.GetUsedMemorySize() == 10);
		for (std::size_t k = 0;k < 20;++k)
		{
			usedSize = 0;
			for (std::size_t i = 0;i < 20;++i)
			{
				usedSize += i * 100;
				allocator.Allocate<std::uint8_t>(i * 100);
			}
			allocator.FinishFrame(k+1);
			auto realUsedSize = allocator.GetUsedMemorySize();
			REQUIRE(realUsedSize >= usedSize);

			allocator.ReleaseFramesBefore(k+1);
			REQUIRE(allocator.GetUsedMemorySize() == 0);
			REQUIRE(allocator.GetInternalBufferCount() == 1);
		}
	}
	*/
}



