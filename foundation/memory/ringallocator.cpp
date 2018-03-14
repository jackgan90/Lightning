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
			if (m_buffer)
			{
				delete[] m_buffer;
				m_buffer = nullptr;
			}
		}

		RingAllocator::RingBuffer::RingBuffer(RingBuffer&& buffer)noexcept:
			m_buffer(buffer.m_buffer), m_maxSize(buffer.m_maxSize), m_usedSize(buffer.m_usedSize)
			,m_head(buffer.m_head), m_tail(buffer.m_tail), m_frameSize(buffer.m_frameSize),
			m_frameMarkers(std::move(buffer.m_frameMarkers))
		{
			buffer.m_buffer = nullptr;
		}

		RingAllocator::RingBuffer& RingAllocator::RingBuffer::operator=(RingBuffer&& buffer)noexcept
		{
			if (&buffer == this)
				return *this;
			m_buffer = buffer.m_buffer;
			m_maxSize = buffer.m_maxSize;
			m_usedSize = buffer.m_usedSize;
			m_head = buffer.m_head;
			m_tail = buffer.m_tail;
			m_frameSize = buffer.m_frameSize;
			m_frameMarkers = std::move(buffer.m_frameMarkers);
			buffer.m_buffer = nullptr;

			return *this;
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

		RingAllocator::RingAllocator():m_lastFinishFrame(0)
		{

		}


		std::uint8_t* RingAllocator::Allocate(std::size_t size)
		{
			if (m_buffers.empty())
			{
				m_buffers.emplace_back(size, m_lastFinishFrame);
			}
			auto& allocBuffer = m_buffers.back();
			auto ptr = allocBuffer.buffer.Allocate(size);
			if (!ptr)
			{
				std::size_t newBufferSize = allocBuffer.buffer.GetSize() * 2;
				while (newBufferSize < size)
					newBufferSize *= 2;
				m_buffers.emplace_back(newBufferSize, m_lastFinishFrame);
				auto& newAllocBuffer = m_buffers.back();
				ptr = newAllocBuffer.buffer.Allocate(size);
			}
			m_buffers.back().lastAllocatedFrame = m_lastFinishFrame;
			return ptr;
		}

		void RingAllocator::ReleaseFramesBefore(std::uint64_t frame)
		{
			std::size_t numBuffersToDelete{ 0 };
			for (std::size_t i = 0;i < m_buffers.size();++i)
			{
				m_buffers[i].buffer.ReleaseFramesBefore(frame);
				if (i == numBuffersToDelete && i < m_buffers.size() - 1 && m_buffers[i].buffer.Empty())//at lease keep one
				{
					numBuffersToDelete++;
				}
			}
			if (numBuffersToDelete)
			{
				m_buffers.erase(m_buffers.begin(), m_buffers.begin() + numBuffersToDelete);
			}
		}

		void RingAllocator::FinishFrame(std::uint64_t frame)
		{
			for (std::size_t i = 0;i < m_buffers.size();++i)
			{
				if (m_buffers[i].lastAllocatedFrame != m_lastFinishFrame)
					continue;
				m_buffers[i].buffer.FinishFrame(frame);
			}
			m_lastFinishFrame = frame;
		}

		std::size_t RingAllocator::GetAllocatedMemorySize()const
		{
			std::size_t totalSize{ 0 };
			std::for_each(m_buffers.cbegin(), m_buffers.cend(), [&](const auto& allocBuffer) {totalSize += allocBuffer.buffer.GetSize(); });
			return totalSize;
		}

		std::size_t RingAllocator::GetUsedMemorySize()const
		{
			std::size_t totalSize{ 0 };
			std::for_each(m_buffers.cbegin(), m_buffers.cend(), [&](const auto& allocBuffer) {totalSize += allocBuffer.buffer.GetUsedSize(); });
			return totalSize;
		}


	}
}