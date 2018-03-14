#pragma once
#include <cstddef>
#include <cstdint>
#include <deque>
#include <vector>

namespace LightningGE
{
	namespace Foundation
	{
		//threaded-unsafe ring allocator.Typycal use in allocation of frame's temp resources
		class RingAllocator
		{
		public:
			std::uint8_t* Allocate(std::size_t size);
			void ReleaseFramesBefore(std::uint64_t frame);
		private:
			class RingBuffer
			{
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
			public:
				RingBuffer(std::size_t size);
				std::uint8_t* Allocate(std::size_t size);
				void FinishFrame(std::uint64_t frame);
				void ReleaseFramesBefore(std::uint64_t frame);
				std::size_t GetSize()const { return m_maxSize; }
				bool Empty()const { return m_usedSize == 0; }
				~RingBuffer();
			private:
				std::uint8_t* m_buffer;
				std::size_t m_maxSize;
				std::size_t m_usedSize;
				std::size_t m_head;
				std::size_t m_tail;
				std::size_t m_frameSize;
				std::deque<FrameMarker> m_frameMarkers;
			};
			std::vector<RingBuffer> m_buffers;
		};
	}
}