#include <vector>
#include <random>
#include <tuple>
#include <sstream>
#include "catch.hpp"
#include "stackallocator.h"

TEST_CASE("Stack allocator allocate test.", "[StackAllocator]") {
	using LightningGE::Foundation::StackAllocator;
	std::tuple<bool, size_t, size_t> params[] = {
		std::make_tuple(true, 16, 4096),
		std::make_tuple(true, 128, 4096),
		std::make_tuple(true, 16, 8192),
		std::make_tuple(false, 16, 4096),
		std::make_tuple(false, 128, 4096),
		std::make_tuple(false, 16, 8192),
	};
	auto paramCount = sizeof(params) / sizeof(std::tuple<bool, size_t, size_t>);
	for (auto i = 0; i < paramCount; ++i)
	{
		auto alignAlloc = std::get<0>(params[i]);
		auto alignment = std::get<1>(params[i]);
		auto blockSize = std::get<2>(params[i]);
		std::stringstream ss;
		ss << alignAlloc << alignment << blockSize;
		auto allocator = new StackAllocator();
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
				for (auto i = 0; i < 100; i++)
					allocMem.push_back(ALLOC(allocator, dist(engine) % 1000, void));
				auto allocatedSize = allocator->GetAllocatedSize();
				auto allocatedCount = allocator->GetAllocatedCount();
				auto usedBlockCount = allocator->GetNonEmptyBlockCount();
				for (auto i = 0; i < allocMem.size(); i++)
				{
					DEALLOC(allocator, allocMem[i]);
					auto blockCount = allocator->GetNonEmptyBlockCount();
					if (blockCount == usedBlockCount)
					{
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
				for (auto i = 0; i < allocMem.size(); i++)
					DEALLOC(allocator, allocMem[i]);
				REQUIRE(allocator->GetAllocatedSize() == 0);
				REQUIRE(allocator->GetAllocatedCount() == 0);
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
