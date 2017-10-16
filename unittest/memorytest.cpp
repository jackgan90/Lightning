#include <cstdlib>
#include <vector>
#include <random>
#include <tuple>
#include <sstream>
#include <iostream>
#include <chrono>
#include <unordered_map>
#include "catch.hpp"
#include "stackallocator.h"

std::tuple<bool, size_t, size_t> params[] = {
	std::make_tuple(true, 16, 4096),
	std::make_tuple(true, 128, 4096),
	std::make_tuple(true, 16, 8192),
	std::make_tuple(false, 16, 4096),
	std::make_tuple(false, 128, 4096),
	std::make_tuple(false, 16, 65536 * 1024),
};

TEST_CASE("Stack allocator allocate test.", "[StackAllocator function]") {
	using LightningGE::Foundation::StackAllocator;
	auto paramCount = sizeof(params) / sizeof(std::tuple<bool, size_t, size_t>);
	for (unsigned int i = 0; i < paramCount; ++i)
	{
		auto alignAlloc = std::get<0>(params[i]);
		auto alignment = std::get<1>(params[i]);
		auto blockSize = std::get<2>(params[i]);
		std::stringstream ss;
		ss << alignAlloc << alignment << blockSize;
		auto allocator = new StackAllocator(alignAlloc, alignment, blockSize);
		SECTION("Alignment test" + ss.str()) {
			if (alignAlloc)
			{
				void* p = ALLOC(allocator, 100, void);
				REQUIRE(reinterpret_cast<size_t>(p) % alignment == 0);
				DEALLOC(allocator, p);
			}
		}
		SECTION("Single allocate and deallocate" + ss.str()) {
			char* mem = ALLOC(allocator, 3000, char);
			REQUIRE(allocator->GetAllocatedSize() == 3000);
			REQUIRE(allocator->GetAllocatedCount() == 1);
			DEALLOC(allocator, mem);
			REQUIRE(allocator->GetAllocatedSize() == 0);
			REQUIRE(allocator->GetAllocatedCount() == 0);

			struct TestStruct 
			{
				std::uint16_t field0;
				std::uint8_t field1;
				std::uint32_t field2;
			};
			TestStruct* ts = ALLOC_ARRAY(allocator, 100, TestStruct);
			REQUIRE(allocator->GetAllocatedSize() == sizeof(TestStruct) * 100);
			DEALLOC(allocator, ts);
			REQUIRE(allocator->GetAllocatedCount() == 0);
			REQUIRE(allocator->GetAllocatedSize() == 0);
		}
		SECTION("Deallocation order" + ss.str()) {
			SECTION("Multiple allocation test") {
				int* mem0 = ALLOC(allocator, sizeof(int) * 200, int);
				char* mem1 = ALLOC(allocator, sizeof(char) * 150, char);
				DEALLOC(allocator, mem0);
				REQUIRE(allocator->GetAllocatedCount() == 2 );
				REQUIRE(allocator->GetAllocatedSize() == (sizeof(int) * 200 + sizeof(char) * 150));
				DEALLOC(allocator, mem1);
				REQUIRE(allocator->GetAllocatedCount() == 0 );
				REQUIRE(allocator->GetAllocatedSize() == 0);

				std::default_random_engine engine;
				std::uniform_int_distribution<int> dist;
				std::vector<void*> allocMem;
				for (unsigned int i = 0; i < 100; i++)
					allocMem.push_back(ALLOC(allocator, dist(engine) % 1000, void));
				auto allocatedSize = allocator->GetAllocatedSize();
				auto allocatedCount = allocator->GetAllocatedCount();
				auto usedBlockCount = allocator->GetNonEmptyBlockCount();
				for (unsigned int i = 0; i < allocMem.size(); i++)
				{
					DEALLOC(allocator, allocMem[i]);
					auto blockCount = allocator->GetNonEmptyBlockCount();
					if (blockCount == usedBlockCount)
					{
						CAPTURE(alignAlloc);
						CAPTURE(alignment);
						CAPTURE(blockSize);
						CAPTURE(blockCount);
						REQUIRE(allocator->GetAllocatedSize() == allocatedSize);
						REQUIRE(allocator->GetAllocatedCount() == allocatedCount);
					}
					else
					{
						usedBlockCount = blockCount;
						allocatedCount = allocator->GetAllocatedCount();
						allocatedSize = allocator->GetAllocatedSize();
					}
				}
				allocMem.clear();
				REQUIRE(allocator->GetAllocatedSize() == 0);
				REQUIRE(allocator->GetAllocatedCount() == 0);

				for (auto i = 0; i < 100; i++)
					allocMem.push_back(ALLOC(allocator, dist(engine) % 1000, void));

				std::random_device rd;
				std::mt19937 g(rd());
				std::shuffle(allocMem.begin(), allocMem.end(), g);
				for (unsigned int i = 0; i < allocMem.size(); i++)
					DEALLOC(allocator, allocMem[i]);
				REQUIRE(allocator->GetAllocatedSize() == 0);
				REQUIRE(allocator->GetAllocatedCount() == 0);
				allocMem.clear();

				//alloc then dealloc , alloc then dealloc
				for (unsigned int i = 0; i < 100; ++i)
					allocMem.push_back(ALLOC(allocator, dist(engine) % 333, void));

				for (unsigned int i = 0; i < 50; ++i)
				{
					DEALLOC(allocator, allocMem[i]);
					allocMem[i] = ALLOC(allocator, dist(engine) % 666, void);
				}

				for (unsigned int i = 0; i < 50; ++i)
				{
					DEALLOC(allocator, allocMem[i]);
				}

				for (unsigned int i = 0; i < 50; ++i)
				{
					allocMem[i] = ALLOC(allocator, dist(engine) % 444, void);
				}

				for (size_t i = 0; i < allocMem.size(); ++i)
				{
					DEALLOC(allocator, allocMem[i]);
				}
				REQUIRE(allocator->GetAllocatedSize() == 0);
				REQUIRE(allocator->GetAllocatedCount() == 0);
				allocMem.clear();
			}
		}
		SECTION("New block allocation test" + ss.str()) {
			auto blockSize = allocator->GetBlockSize();
			char* mem0 = ALLOC(allocator, static_cast<size_t>(2.0 * blockSize / 3), char);
			REQUIRE(allocator->GetNonEmptyBlockCount() == 1);
			char* mem1 = ALLOC(allocator, static_cast<size_t>(blockSize / 2), char);
			REQUIRE(allocator->GetNonEmptyBlockCount() == 2);
			DEALLOC(allocator, mem0);
			REQUIRE(allocator->GetNonEmptyBlockCount() == 1);
			DEALLOC(allocator, mem1);
			REQUIRE(allocator->GetNonEmptyBlockCount() == 0);
		}
		delete allocator;
	}
}

TEST_CASE("Stack allocator performance test", "[StackAllocator performance]") {
	constexpr const int AllocateCount = 30000;
	constexpr const int AllocateUnit = 100;
	std::cout << "====================StackAllocator performance test start========================" << std::endl;
	std::cout << "Allocate "<< AllocateUnit << " bytes " << AllocateCount << " times" << std::endl;
	using std::chrono::duration;
	using std::chrono::duration_cast;
	void* memArray[AllocateCount];
	auto paramCount = sizeof(params) / sizeof(std::tuple<bool, size_t, size_t>);
	for (unsigned int i = 0; i < paramCount; ++i)
	{
		auto alignAlloc = std::get<0>(params[i]);
		auto alignment = std::get<1>(params[i]);
		auto blockSize = std::get<2>(params[i]);
		std::stringstream ss;
		ss << alignAlloc << alignment << blockSize;
		LightningGE::Foundation::StackAllocator allocator(alignAlloc, alignment, blockSize);
		SECTION("PerfTest" + ss.str())
		{
			std::cout << "alignAlloc:" << std::boolalpha << alignAlloc << ",alignment:" << alignment << ",blockSize:" << blockSize << std::endl;
			auto malloc_start = std::chrono::high_resolution_clock::now();
			for (auto i = 0; i < AllocateCount; ++i)
				memArray[i] = std::malloc(AllocateUnit);
			auto malloc_end = std::chrono::high_resolution_clock::now();

			std::cout << "[std::malloc allocation time:] " << duration_cast<duration<double>>(malloc_end - malloc_start).count() << std::endl;

			auto free_start = std::chrono::high_resolution_clock::now();
			for (auto i = 0; i < AllocateCount; ++i)
				std::free(memArray[i]);
			auto free_end = std::chrono::high_resolution_clock::now();

			std::cout << "[std::free deallocation time:] " << duration_cast<duration<double>>(free_end - free_start).count() << std::endl;

			auto new_start = std::chrono::high_resolution_clock::now();
			for (auto i = 0; i < AllocateCount; ++i)
				memArray[i] = reinterpret_cast<void*>(new char[AllocateUnit]);
			auto new_end = std::chrono::high_resolution_clock::now();

			std::cout << "[new allocation time:] " << duration_cast<duration<double>>(new_end - new_start).count() << std::endl;

			auto delete_start = std::chrono::high_resolution_clock::now();
			for (auto i = 0; i < AllocateCount; ++i)
				delete[] memArray[i];
			auto delete_end = std::chrono::high_resolution_clock::now();
			std::cout << "[delete deallocation time:] " << duration_cast<duration<double>>(delete_end - delete_start).count() << std::endl;

			auto allocator_alloc_start = std::chrono::high_resolution_clock::now();
			for (auto i = 0; i < AllocateCount; ++i)
				memArray[i] = ALLOC(&allocator, AllocateUnit, void);
			auto allocator_alloc_end = std::chrono::high_resolution_clock::now();

			std::cout << "[allocator allocation time:] " << duration_cast<duration<double>>(allocator_alloc_end - allocator_alloc_start).count() << std::endl;
			//std::cout << "Allocator block count: " << allocator.GetBlockCount() << std::endl;

			auto allocator_dealloc_start = std::chrono::high_resolution_clock::now();
			for (auto i = 0; i < AllocateCount; ++i)
				DEALLOC(&allocator, memArray[i]);
			auto allocator_dealloc_end = std::chrono::high_resolution_clock::now();

			std::cout << "[allocator deallocation time:] " << duration_cast<duration<double>>(allocator_dealloc_end - allocator_dealloc_start).count() << std::endl;

			std::cout << "====================StackAllocator performance test end==========================" << std::endl;
		}
	}


}

TEST_CASE("Unordered map get performance test") {
	using LightningGE::Foundation::MemoryInfo;
	using std::chrono::duration;
	using std::chrono::duration_cast;
	std::cout << "Unordered map operator[] performance test" << std::endl;
	constexpr const int AllocateCount = 1000;
	std::unordered_map<void*, MemoryInfo*> info;
	for (unsigned int i = 0; i < AllocateCount; ++i)
		info.insert(std::make_pair(new char, new MemoryInfo));
	auto map_traverse_start = std::chrono::high_resolution_clock::now();
	for (auto it = info.begin(); it != info.end(); ++it)
	{
		auto val = info[it->first];
	}
	auto map_traverse_end = std::chrono::high_resolution_clock::now();

	std::cout << "[map traverse time:] " << duration_cast<duration<double>>(map_traverse_end - map_traverse_start).count() << std::endl;
}
