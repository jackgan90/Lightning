#include <algorithm>
#include "ringallocator.h"

namespace Lightning
{
	namespace Foundation
	{
		RingAllocator::RingBuffer::RingBuffer(std::size_t size):
			mMaxSize(size < MIN_BUFFER_SIZE ? MIN_BUFFER_SIZE : size), mUsedSize(0), mHead(0), mTail(0), mFrameSize(0)
		{
			mBuffer = new std::uint8_t[mMaxSize];
		}

		RingAllocator::RingBuffer::~RingBuffer()
		{
			if (mBuffer)
			{
				delete[] mBuffer;
				mBuffer = nullptr;
			}
		}

		RingAllocator::RingBuffer::RingBuffer(RingBuffer&& buffer)noexcept:
			mBuffer(buffer.mBuffer), mMaxSize(buffer.mMaxSize), mUsedSize(buffer.mUsedSize)
			,mHead(buffer.mHead), mTail(buffer.mTail), mFrameSize(buffer.mFrameSize),
			mFrameMarkers(std::move(buffer.mFrameMarkers))
		{
			buffer.mBuffer = nullptr;
		}

		RingAllocator::RingBuffer& RingAllocator::RingBuffer::operator=(RingBuffer&& buffer)noexcept
		{
			if (&buffer == this)
				return *this;
			mBuffer = buffer.mBuffer;
			mMaxSize = buffer.mMaxSize;
			mUsedSize = buffer.mUsedSize;
			mHead = buffer.mHead;
			mTail = buffer.mTail;
			mFrameSize = buffer.mFrameSize;
			mFrameMarkers = std::move(buffer.mFrameMarkers);
			buffer.mBuffer = nullptr;

			return *this;
		}

		std::uint8_t* RingAllocator::RingBuffer::Allocate(std::size_t size)
		{
			if (mUsedSize >= mMaxSize)
			{
				return nullptr;
			}
			if (mTail >= mHead)		//tail after head,in early several allocations this is the case
			{
				if (mTail + size <= mMaxSize)	//try allocate after mTail, will allocate (mTail, mTail + size) memory
				{
					auto offset = mTail;
					mTail += size;
					mUsedSize += size;
					mFrameSize += size;
					return mBuffer + offset;
				}
				else			//if cannot allocate after mTail,check if it's able to allocate from the start of the buffer
				{
					if (size <= mHead)
					{
						auto realSize = (mMaxSize - mTail) + size;
						mUsedSize += realSize;
						mFrameSize += realSize;
						mTail = size;
						return mBuffer;
					}
				}
			}
			else				//tail before head, which means the tail has wrapped around the end,available space is (tail, head)
			{
				if (mTail + size <= mHead)
				{
					auto offset = mTail;
					mTail += size;
					mUsedSize += size;
					mFrameSize += size;
					return mBuffer + offset;
				}
			}
			return nullptr;
		}

		void RingAllocator::RingBuffer::FinishFrame(std::uint64_t frame)
		{
			mFrameMarkers.emplace_back(mTail, mFrameSize, frame);
			mFrameSize = 0;
		}

		void RingAllocator::RingBuffer::ReleaseFramesBefore(std::uint64_t frame)
		{
			while (!mFrameMarkers.empty() && mFrameMarkers.front().frame <= frame)
			{
				const auto& marker = mFrameMarkers.front();
				mUsedSize -= marker.size;
				mHead = marker.offset;
				mFrameMarkers.pop_front();
			}
		}

		RingAllocator::RingAllocator():mLastFinishFrame(0)
		{

		}


		std::uint8_t* RingAllocator::AllocateBytes(std::size_t size)
		{
			if (mBuffers.empty())
			{
				mBuffers.emplace_back(size, mLastFinishFrame);
			}
			auto& allocBuffer = mBuffers.back();
			auto ptr = allocBuffer.buffer.Allocate(size);
			if (!ptr)
			{
				std::size_t newBufferSize = allocBuffer.buffer.GetSize() * 2;
				while (newBufferSize < size)
					newBufferSize *= 2;
				mBuffers.emplace_back(newBufferSize, mLastFinishFrame);
				auto& newAllocBuffer = mBuffers.back();
				ptr = newAllocBuffer.buffer.Allocate(size);
			}
			mBuffers.back().lastAllocatedFrame = mLastFinishFrame;
			return ptr;
		}

		void RingAllocator::ReleaseFramesBefore(std::uint64_t frame)
		{
			std::size_t numBuffersToDelete{ 0 };
			for (std::size_t i = 0;i < mBuffers.size();++i)
			{
				mBuffers[i].buffer.ReleaseFramesBefore(frame);
				if (i < mBuffers.size() - 1 && mBuffers[i].buffer.Empty())//at lease keep one
				{
					numBuffersToDelete++;
				}
			}
			if (numBuffersToDelete)
			{
				mBuffers.erase(mBuffers.begin(), mBuffers.begin() + numBuffersToDelete);
			}
		}

		void RingAllocator::FinishFrame(std::uint64_t frame)
		{
			for (std::size_t i = 0;i < mBuffers.size();++i)
			{
				if (mBuffers[i].lastAllocatedFrame != mLastFinishFrame)
					continue;
				mBuffers[i].buffer.FinishFrame(frame);
			}
			mLastFinishFrame = frame;
		}

		std::size_t RingAllocator::GetAllocatedMemorySize()const
		{
			std::size_t totalSize{ 0 };
			std::for_each(mBuffers.cbegin(), mBuffers.cend(), [&](const auto& allocBuffer) {totalSize += allocBuffer.buffer.GetSize(); });
			return totalSize;
		}

		std::size_t RingAllocator::GetUsedMemorySize()const
		{
			std::size_t totalSize{ 0 };
			std::for_each(mBuffers.cbegin(), mBuffers.cend(), [&](const auto& allocBuffer) {totalSize += allocBuffer.buffer.GetUsedSize(); });
			return totalSize;
		}


	}
}