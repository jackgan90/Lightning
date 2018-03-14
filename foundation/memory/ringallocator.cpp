#include <algorithm>
#include "ringallocator.h"

namespace LightningGE
{
	namespace Foundation
	{
		RingAllocator::RingBuffer::RingBuffer(std::size_t size):
			m_maxSize(size < MIN_BUFFER_SIZE ? MIN_BUFFER_SIZE : size), m_usedSize(0), m_head(0), m_tail(0), m_frameSize(0)
		{
			m_buffer = new std::uint8_t[m_maxSize];
		}

		RingAllocator::RingBuffer::~RingBuffer()
		{
			delete[] m_buffer;
		}

		std::uint8_t* RingAllocator::RingBuffer::Allocate(std::size_t size)
		{
			if (m_usedSize >= m_maxSize)
			{
				return nullptr;
			}
			if (m_tail >= m_head)		//tail after head,in early several allocations this is the case
			{
				if (m_tail + size <= m_maxSize)	//try allocate after m_tail, will allocate (m_tail, m_tail + size) memory
				{
					auto offset = m_tail;
					m_tail += size;
					m_usedSize += size;
					m_frameSize += size;
					return m_buffer + offset;
				}
				else			//if cannot allocate after m_tail,check if it's able to allocate from the start of the buffer
				{
					if (size <= m_head)
					{
						auto realSize = (m_maxSize - m_tail) + size;
						m_usedSize += realSize;
						m_frameSize += realSize;
						m_tail = size;
						return m_buffer;
					}
				}
			}
			else				//tail before head, which means the tail has wrapped around the end,available space is (tail, head)
			{
				if (m_tail + size <= m_head)
				{
					auto offset = m_tail;
					m_tail += size;
					m_usedSize += size;
					m_frameSize += size;
					return m_buffer + offset;
				}
			}
			return nullptr;
		}

		void RingAllocator::RingBuffer::FinishFrame(std::uint64_t frame)
		{
			m_frameMarkers.emplace_back(m_tail, m_frameSize, frame);
			m_frameSize = 0;
		}

		void RingAllocator::RingBuffer::ReleaseFramesBefore(std::uint64_t frame)
		{
			while (!m_frameMarkers.empty() && m_frameMarkers.front().frame <= frame)
			{
				const auto& marker = m_frameMarkers.front();
				m_usedSize -= marker.size;
				m_head = marker.offset;
				m_frameMarkers.pop_front();
			}
		}

		std::uint8_t* RingAllocator::Allocate(std::size_t size)
		{
			if (m_buffers.empty())
			{
				m_buffers.emplace_back(size);
			}
			auto& buffer = m_buffers.back();
			auto ptr = buffer.Allocate(size);
			if (!ptr)
			{
				std::size_t newBufferSize = buffer.GetSize() * 2;
				while (newBufferSize < size)
					newBufferSize *= 2;
				m_buffers.emplace_back(newBufferSize);
				ptr = m_buffers.back().Allocate(size);
			}
			return ptr;
		}

		void RingAllocator::ReleaseFramesBefore(std::uint64_t frame)
		{
			std::for_each(m_buffers.begin(), m_buffers.end(), [frame](RingAllocator::RingBuffer& buffer) {buffer.ReleaseFramesBefore(frame); });
			std::remove_if(m_buffers.begin(), m_buffers.end(), [&](const RingAllocator::RingBuffer& buffer) {return buffer.Empty(); });
		}
	}
}