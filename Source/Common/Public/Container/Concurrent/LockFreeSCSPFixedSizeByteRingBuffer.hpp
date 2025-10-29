/*
Copyright 2025 creatorlxd

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#pragma once
#include <cstring>
#include "MemoryManager.h"
#include "Error.h"
#include "Concurrent/Atomic.hpp"

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{
	/*!
	@brief Lock-free single consumer and single producer fixed-size ring-buffer.
	*/
	template<SizeType BufferSize, IsAllocator Allocator = DefaultAllocator>
	class LockFreeSCSPFixedSizeByteRingBuffer
	{
	public:
		inline static constexpr const SizeType BufferSize = BufferSize;
		using AllocatorType = Allocator;

		template<SizeType _BufferSize, IsAllocator _Allocator>
		friend class LockFreeSCSPFixedSizeByteRingBuffer;

		inline LockFreeSCSPFixedSizeByteRingBuffer()
			: m_pContent(Allocator::RawNew(BufferSize)), m_Head(0), m_Tail(0)
		{
		}

		inline ~LockFreeSCSPFixedSizeByteRingBuffer()
		{
			void* ptr = m_pContent.Load(MemoryOrder::Acquire);
			if (ptr)
				Allocator::RawDelete(ptr);
		}

		inline LockFreeSCSPFixedSizeByteRingBuffer(const LockFreeSCSPFixedSizeByteRingBuffer& other)
			: m_Head(other.m_Head.Load(MemoryOrder::Acquire)), m_Tail(other.m_Tail.Load(MemoryOrder::Acquire))
		{
			void* ptr = Allocator::RawNew(BufferSize);
			memcpy(ptr, other.m_pContent.Load(MemoryOrder::Acquire), BufferSize);
			m_pContent.Store(ptr, MemoryOrder::Release);
		}

		inline LockFreeSCSPFixedSizeByteRingBuffer(LockFreeSCSPFixedSizeByteRingBuffer&& other)
		{
			m_Head.Store(other.m_Head.Exchange(0, MemoryOrder::AcquireRelease), MemoryOrder::Release);
			m_Tail.Store(other.m_Tail.Exchange(0, MemoryOrder::AcquireRelease), MemoryOrder::Release);
			m_pContent.Store(other.m_pContent.Exchange(nullptr, MemoryOrder::AcquireRelease), MemoryOrder::Release);
		}

		inline LockFreeSCSPFixedSizeByteRingBuffer& operator=(const LockFreeSCSPFixedSizeByteRingBuffer& other)
		{
			SGE_ASSERT(SelfAssignmentError, this, &other);
			m_Head.Store(other.m_Head.Load(MemoryOrder::Acquire), MemoryOrder::Release);
			m_Tail.Store(other.m_Tail.Load(MemoryOrder::Acquire), MemoryOrder::Release);
			memcpy(m_pContent.Load(MemoryOrder::Acquire), other.m_pContent.Load(MemoryOrder::Acquire), BufferSize);
			return *this;
		}

		inline LockFreeSCSPFixedSizeByteRingBuffer& operator=(LockFreeSCSPFixedSizeByteRingBuffer&& other)
		{
			SGE_ASSERT(SelfAssignmentError, this, &other);
			m_Head.Store(other.m_Head.Exchange(0, MemoryOrder::AcquireRelease), MemoryOrder::Release);
			m_Tail.Store(other.m_Tail.Exchange(0, MemoryOrder::AcquireRelease), MemoryOrder::Release);
			m_pContent.Store(other.m_pContent.Exchange(nullptr, MemoryOrder::AcquireRelease), MemoryOrder::Release);
			return *this;
		}

		template<typename OtherAllocator>
		inline LockFreeSCSPFixedSizeByteRingBuffer(const LockFreeSCSPFixedSizeByteRingBuffer<BufferSize, OtherAllocator>& other)
			: m_Head(other.m_Head.Load(MemoryOrder::Acquire)), m_Tail(other.m_Tail.Load(MemoryOrder::Acquire))
		{
			void* ptr = Allocator::RawNew(BufferSize);
			memcpy(ptr, other.m_pContent.Load(MemoryOrder::Acquire), BufferSize);
			m_pContent.Store(ptr, MemoryOrder::Release);
		}

		template<typename OtherAllocator>
		inline LockFreeSCSPFixedSizeByteRingBuffer(LockFreeSCSPFixedSizeByteRingBuffer<BufferSize, OtherAllocator>&& other)
			: m_Head(other.m_Head.Load(MemoryOrder::Acquire)), m_Tail(other.m_Tail.Load(MemoryOrder::Acquire))
		{
			void* ptr = Allocator::RawNew(BufferSize);
			memcpy(ptr, other.m_pContent.Load(MemoryOrder::Acquire), BufferSize);
			m_pContent.Store(ptr, MemoryOrder::Release);
		}

		template<typename OtherAllocator>
		inline LockFreeSCSPFixedSizeByteRingBuffer& operator=(const LockFreeSCSPFixedSizeByteRingBuffer<BufferSize, OtherAllocator>& other)
		{
			m_Head.Store(other.m_Head.Load(MemoryOrder::Acquire), MemoryOrder::Release);
			m_Tail.Store(other.m_Tail.Load(MemoryOrder::Acquire), MemoryOrder::Release);
			memcpy(m_pContent.Load(MemoryOrder::Acquire), other.m_pContent.Load(MemoryOrder::Acquire), BufferSize);
			return *this;
		}

		template<typename OtherAllocator>
		inline LockFreeSCSPFixedSizeByteRingBuffer& operator=(LockFreeSCSPFixedSizeByteRingBuffer<BufferSize, OtherAllocator>&& other)
		{
			m_Head.Store(other.m_Head.Load(MemoryOrder::Acquire), MemoryOrder::Release);
			m_Tail.Store(other.m_Tail.Load(MemoryOrder::Acquire), MemoryOrder::Release);
			memcpy(m_pContent.Load(MemoryOrder::Acquire), other.m_pContent.Load(MemoryOrder::Acquire), BufferSize);
			return *this;
		}

		inline bool TryPush(const void* ptr, SizeType size)
		{
			SGE_ASSERT(NullPointerError, ptr);
			SizeType tail = m_Tail.Load(MemoryOrder::Acquire);
			SizeType head = m_Head.Load(MemoryOrder::Acquire);
			if (tail + size > head + BufferSize)
				return false;
			OperateOnBuffer(tail, size, [this, &ptr](void* cur_ptr, SizeType cur_size) {
				memcpy(cur_ptr, ptr, cur_size);
				ptr = (const Byte*)ptr + cur_size;
			});
			m_Tail.Store(tail + size, MemoryOrder::Release);
			return true;
		}

		/*!
		@brief Read the top `size` bytes from the ring buffer.
		@note This function only reads data without removing it from the ring buffer. Since data may be splited, the callback function may be invoked twice with different chunk sizes, though their sum will equal the requested size parameter.
		@param size The size of the data to be read from the top of the ring buffer.
		@param func The function to be called on the top data. It should accept a pointer to the data and its size.
		@return The actual size of the data read from the ring buffer, which may be less than the requested size if there is not enough data available.
		*/
		template<typename Func>
		inline SizeType Top(SizeType size, Func&& func)
		{
			SizeType head = m_Head.Load(MemoryOrder::Acquire);
			SizeType tail = m_Tail.Load(MemoryOrder::Acquire);
			SizeType real_size = Min(size, tail - head);
			if (real_size)
				OperateOnBuffer(m_Head.Load(MemoryOrder::Acquire), real_size, std::forward<Func>(func));
			return real_size;
		}

		/*!
		@brief Pop data from the ring buffer.
		@note This function reads data and then removes it from the ring buffer. Since data may be splited, the callback function may be invoked twice with different chunk sizes, though their sum will equal the requested size parameter.
		@param func The function to be called on the popped data. It should accept a pointer to the data and its size.
		@return The actual size of the data popped from the ring buffer.
		*/
		template<typename Func>
		inline SizeType Pop(SizeType size, Func&& func)
		{
			SizeType head = m_Head.Load(MemoryOrder::Acquire);
			SizeType tail = m_Tail.Load(MemoryOrder::Acquire);
			SizeType real_size = Min(size, tail - head);
			if (real_size)
			{
				OperateOnBuffer(head, real_size, std::forward<Func>(func));
				m_Head.Store(head + real_size, MemoryOrder::Release);
			}
			return real_size;
		}

		inline SizeType GetSize() const
		{
			return m_Tail.Load(MemoryOrder::Acquire) - m_Head.Load(MemoryOrder::Acquire);
		}

		inline SizeType GetFreeSize() const
		{
			return BufferSize - GetSize();
		}

		inline void Clear()
		{
			m_Head.Store(0, MemoryOrder::Release);
			m_Tail.Store(0, MemoryOrder::Release);
		}

	private:
		/*!
		@brief Operate on the buffer at the given index.
		@note This function will call the provided function `func` on the buffer at the specified index. It assumes that the index is valid and that the size of the data to be operated on does not exceed the buffer size(touchs the head).
		@param index The index of the buffer to operate on.
		@param size The size of the data to be operated on.
		@param func The function to be called on the buffer. (ptr, size)
		*/
		template<typename Func>
		inline void OperateOnBuffer(SizeType index, SizeType size, Func&& func)
		{
			SizeType cur_buffer_idx = index % BufferSize;
			while (size)
			{
				SizeType cur_size = Min(size, BufferSize - cur_buffer_idx);
				func((Byte*)(m_pContent.Load(MemoryOrder::Acquire)) + cur_buffer_idx, cur_size);
				size -= cur_size;
				cur_buffer_idx = 0;
			}
		}

	private:
		Atomic<void*> m_pContent;
		Atomic<SizeType> m_Head;
		Atomic<SizeType> m_Tail;
	};
}

/*!
@}
*/