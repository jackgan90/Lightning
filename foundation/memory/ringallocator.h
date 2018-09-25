#pragma once
#include <cstddef>
#include <cstdint>
#include <thread>
#include <mutex>
#include "container.h"
#include "foundationexportdef.h"

namespace Lightning
{
	namespace Foundation
	{
		//threaded-safe ring allocator.Typycal use in allocation of frame's temp resources
		class LIGHTNING_FOUNDATION_API RingAllocator
		{
		public:
			RingAllocator();
			~RingAllocator() = default;
			RingAllocator(const RingAllocator&) = delete;
			RingAllocator& operator=(const RingAllocator&) = delete;
			//Allocate elementCount elements of type T ,should not run simultaineously with FinishFrame and ReleaseFramesBefore
			template<typename T, typename... Args>
			T* Allocate(std::size_t elementCount, Args&&... args)
			{
				auto ptr = AllocateBytes(elementCount * sizeof(T));
				for (std::size_t i = 0;i < elementCount;++i)
				{
					new (ptr + i * sizeof(T)) T(std::forward<Args>(args)...);
				}
				return reinterpret_cast<T*>(ptr);
			}
			std::size_t GetAllocatedMemorySize()const;
			std::size_t GetUsedMemorySize()const;
			void ReleaseFramesBefore(std::uint64_t frame);
			void FinishFrame(std::uint64_t frame);
			std::size_t GetInternalBufferCount()const { return mBuffers.size(); }
		private:
			std::uint8_t* AllocateBytes(std::size_t size);
			class RingBuffer
			{
			public:
				RingBuffer(std::size_t size);
				RingBuffer(const RingBuffer&) = delete;
				RingBuffer& operator=(const RingBuffer&) = delete;
				RingBuffer(RingBuffer&&)noexcept;
				RingBuffer& operator=(RingBuffer&&)noexcept;
				std::uint8_t* Allocate(std::size_t size);
				void FinishFrame(std::uint64_t frame);
				void ReleaseFramesBefore(std::uint64_t frame);
				std::size_t GetSize()const { return mMaxSize; }
				std::size_t GetUsedSize()const { return mUsedSize; }
				bool Empty()const { return mUsedSize == 0; }
				~RingBuffer();
			private:
				static constexpr std::size_t MIN_BUFFER_SIZE = 2048;
				struct FrameMarker
				{
					FrameMarker(std::size_t _offset, std::size_t _size, std::uint64_t _frame):
						offset(_offset), size(_size), frame(_frame){}
					std::size_t offset;
					std::size_t size;
					std::uint64_t frame;
				};
				std::uint8_t* mBuffer;
				std::size_t mMaxSize;
				std::size_t mUsedSize;
				std::size_t mHead;
				std::size_t mTail;
				std::size_t mFrameSize;
				container::deque<FrameMarker> mFrameMarkers;
			};

			struct RingBufferAllocation
			{
				RingBufferAllocation(std::size_t size, std::uint64_t frame):buffer(size), lastAllocatedFrame(frame){}
				RingBuffer buffer;
				std::uint64_t lastAllocatedFrame;
			};
			container::unordered_map<std::thread::id, container::vector<RingBufferAllocation>> mBuffers;
			std::uint64_t mLastFinishFrame;
			static std::mutex sBufferMutex;
		};
	}
}
