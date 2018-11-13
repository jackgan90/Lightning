#include <algorithm>
#include "framememoryallocator.h"

namespace Lightning
{
	namespace Foundation
	{
		FrameMemoryAllocator::RingBuffer::RingBuffer(std::size_t size):
			mMaxSize(size < MIN_BUFFER_SIZE ? MIN_BUFFER_SIZE : size), mUsedSize(0), mHead(0), mTail(0), mFrameSize(0)
		{
			mBuffer = new std::uint8_t[mMaxSize];
		}

		FrameMemoryAllocator::RingBuffer::~RingBuffer()
		{
			if (mBuffer)
			{
				delete[] mBuffer;
				mBuffer = nullptr;
			}
		}

		FrameMemoryAllocator::RingBuffer::RingBuffer(RingBuffer&& buffer)noexcept:
			mBuffer(buffer.mBuffer), mMaxSize(buffer.mMaxSize), mUsedSize(buffer.mUsedSize)
			,mHead(buffer.mHead), mTail(buffer.mTail), mFrameSize(buffer.mFrameSize),
			mFrameMarkers(std::move(buffer.mFrameMarkers))
		{
			buffer.mBuffer = nullptr;
		}

		FrameMemoryAllocator::RingBuffer& FrameMemoryAllocator::RingBuffer::operator=(RingBuffer&& buffer)noexcept
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

		std::uint8_t* FrameMemoryAllocator::RingBuffer::Allocate(std::size_t size)
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

		void FrameMemoryAllocator::RingBuffer::FinishFrame(std::uint64_t frame)
		{
			mFrameMarkers.emplace_back(mTail, mFrameSize, frame);
			mFrameSize = 0;
		}

		void FrameMemoryAllocator::RingBuffer::ReleaseFramesBefore(std::uint64_t frame)
		{
			while (!mFrameMarkers.empty() && mFrameMarkers.front().frame <= frame)
			{
				const auto& marker = mFrameMarkers.front();
				mUsedSize -= marker.size;
				mHead = marker.offset;
				mFrameMarkers.pop_front();
			}
		}

		FrameMemoryAllocator::FrameMemoryAllocator()
		{

		}


		std::uint8_t* FrameMemoryAllocator::AllocateBytes(std::size_t size)
		{
			auto thread_id = std::this_thread::get_id();
			auto& threadBuffers = *mBuffers;
			if (threadBuffers.empty())
			{
				threadBuffers.emplace_back(size);
			}
			auto& allocBuffer = threadBuffers.back();
			auto ptr = allocBuffer.Allocate(size);
			if (!ptr)
			{
				std::size_t newBufferSize = allocBuffer.GetSize() * 2;
				while (newBufferSize < size)
					newBufferSize *= 2;
				threadBuffers.emplace_back(newBufferSize);
				auto& newAllocBuffer = threadBuffers.back();
				ptr = newAllocBuffer.Allocate(size);
			}
			return ptr;
		}

		void FrameMemoryAllocator::ReleaseFramesBefore(std::uint64_t frame)
		{
			mBuffers.for_each([this, frame](Container::Vector<RingBuffer>& threadBuffers) {
				std::size_t numBuffersToDelete{ 0 };
				for (std::size_t i = 0;i < threadBuffers.size();++i)
				{
					threadBuffers[i].ReleaseFramesBefore(frame);
					if (i < threadBuffers.size() - 1 && threadBuffers[i].Empty())//at lease keep one
					{
						numBuffersToDelete++;
					}
				}
				if (numBuffersToDelete)
				{
					threadBuffers.erase(threadBuffers.begin(), threadBuffers.begin() + numBuffersToDelete);
				}
			});
		}

		void FrameMemoryAllocator::FinishFrame(std::uint64_t frame)
		{
			for (auto it = mBuffers.begin(); it != mBuffers.end(); ++it)
			{
				auto& threadBuffers = *it;
				for (std::size_t i = 0;i < threadBuffers.size();++i)
				{
					threadBuffers[i].FinishFrame(frame);
				}
			}
		}

		std::size_t FrameMemoryAllocator::GetAllocatedMemorySize()const
		{
			std::size_t totalSize{ 0 };
			mBuffers.for_each([&totalSize](const Container::Vector<RingBuffer>& threadBuffers) {
				for (const auto& buffer : threadBuffers)
				{
					totalSize += buffer.GetSize();
				}
			});
			return totalSize;
		}

		std::size_t FrameMemoryAllocator::GetUsedMemorySize()const
		{
			std::size_t totalSize{ 0 };
			mBuffers.for_each([&totalSize](const Container::Vector<RingBuffer>& threadBuffers) {
				for (const auto& buffer : threadBuffers)
				{
					totalSize += buffer.GetUsedSize();
				}
			});
			return totalSize;
		}


	}
}