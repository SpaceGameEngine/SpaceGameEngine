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

	template<SizeType MaxSize, IsAllocator Allocator = DefaultAllocator>
	class LockFreeFixedSizeBuffer
	{
	public:
		inline static constexpr const SizeType sm_MaxSize = MaxSize;
		using AllocatorType = Allocator;

		struct BufferOverflowError
		{
			inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The buffer is overflow.");
			inline static bool Judge(SizeType size)
			{
				return size > MaxSize;
			}
		};

		template<SizeType _MaxSize, IsAllocator _Allocator>
		friend class LockFreeFixedSizeBuffer;

		inline LockFreeFixedSizeBuffer()
			: m_pContent(Allocator::RawNew(MaxSize)), m_Size(0)
		{
		}

		inline ~LockFreeFixedSizeBuffer()
		{
			auto ptr = m_pContent.Load(MemoryOrder::Acquire);
			if (ptr)
				Allocator::RawDelete(ptr);
		}

		inline LockFreeFixedSizeBuffer(const LockFreeFixedSizeBuffer& buffer)
			: m_pContent(Allocator::RawNew(MaxSize))
		{
			void* ptr = buffer.m_pContent.Load(MemoryOrder::Acquire);
			SizeType size = buffer.m_Size.Load(MemoryOrder::Acquire);
			memcpy(m_pContent.Load(MemoryOrder::Acquire), ptr, size);
			m_Size.Store(size, MemoryOrder::Release);
		}

		inline LockFreeFixedSizeBuffer(LockFreeFixedSizeBuffer&& buffer)
			: m_pContent(buffer.m_pContent.Exchange(nullptr, MemoryOrder::AcquireRelease)), m_Size(buffer.m_Size.Load(MemoryOrder::Acquire))
		{
		}

		inline LockFreeFixedSizeBuffer& operator=(const LockFreeFixedSizeBuffer& buffer)
		{
			SGE_ASSERT(SelfAssignmentError, this, &buffer);
			void* ptr = buffer.m_pContent.Load(MemoryOrder::Acquire);
			SizeType size = buffer.m_Size.Load(MemoryOrder::Acquire);
			memcpy(m_pContent.Load(MemoryOrder::Acquire), ptr, size);
			m_Size.Store(size, MemoryOrder::Release);
			return *this;
		}

		inline LockFreeFixedSizeBuffer& operator=(LockFreeFixedSizeBuffer&& buffer)
		{
			SGE_ASSERT(SelfAssignmentError, this, &buffer);
			auto ptr = m_pContent.Exchange(buffer.m_pContent.Exchange(nullptr, MemoryOrder::AcquireRelease), MemoryOrder::AcquireRelease);
			if (ptr)
				Allocator::RawDelete(ptr);
			m_Size.Store(buffer.m_Size.Load(MemoryOrder::Acquire), MemoryOrder::Release);
			return *this;
		}

		template<typename OtherAllocator>
		inline LockFreeFixedSizeBuffer(const LockFreeFixedSizeBuffer<MaxSize, OtherAllocator>& buffer)
			: m_pContent(Allocator::RawNew(MaxSize))
		{
			void* ptr = buffer.m_pContent.Load(MemoryOrder::Acquire);
			SizeType size = buffer.m_Size.Load(MemoryOrder::Acquire);
			memcpy(m_pContent.Load(MemoryOrder::Acquire), ptr, size);
			m_Size.Store(size, MemoryOrder::Release);
		}

		template<typename OtherAllocator>
		inline LockFreeFixedSizeBuffer(LockFreeFixedSizeBuffer<MaxSize, OtherAllocator>&& buffer)
			: m_pContent(Allocator::RawNew(MaxSize))
		{
			void* ptr = buffer.m_pContent.Load(MemoryOrder::Acquire);
			SizeType size = buffer.m_Size.Load(MemoryOrder::Acquire);
			memcpy(m_pContent.Load(MemoryOrder::Acquire), ptr, size);
			m_Size.Store(size, MemoryOrder::Release);
		}

		template<typename OtherAllocator>
		inline LockFreeFixedSizeBuffer& operator=(const LockFreeFixedSizeBuffer<MaxSize, OtherAllocator>& buffer)
		{
			void* ptr = buffer.m_pContent.Load(MemoryOrder::Acquire);
			SizeType size = buffer.m_Size.Load(MemoryOrder::Acquire);
			memcpy(m_pContent.Load(MemoryOrder::Acquire), ptr, size);
			m_Size.Store(size, MemoryOrder::Release);
			return *this;
		}

		template<typename OtherAllocator>
		inline LockFreeFixedSizeBuffer& operator=(LockFreeFixedSizeBuffer<MaxSize, OtherAllocator>&& buffer)
		{
			void* ptr = buffer.m_pContent.Load(MemoryOrder::Acquire);
			SizeType size = buffer.m_Size.Load(MemoryOrder::Acquire);
			memcpy(m_pContent.Load(MemoryOrder::Acquire), ptr, size);
			m_Size.Store(size, MemoryOrder::Release);
			return *this;
		}

		inline void* GetData()
		{
			return m_pContent.Load(MemoryOrder::Acquire);
		}

		inline const void* GetData() const
		{
			return m_pContent.Load(MemoryOrder::Acquire);
		}

		inline void SetSize(SizeType size)
		{
			SGE_ASSERT(BufferOverflowError, size);
			m_Size.Store(size, MemoryOrder::Release);
		}

		inline SizeType GetSize() const
		{
			return m_Size.Load(MemoryOrder::Acquire);
		}

		inline SizeType GetFreeSize() const
		{
			SizeType size = m_Size.Load(MemoryOrder::Acquire);
			SGE_ASSERT(BufferOverflowError, size);
			return MaxSize - size;
		}

		inline void Clear()
		{
			m_Size.Store(0, MemoryOrder::Release);
		}

		inline bool TryAppend(const void* ptr, SizeType size)
		{
			SGE_ASSERT(NullPointerError, ptr);
			SizeType cur_size = m_Size.Load(MemoryOrder::Acquire);
			if (BufferOverflowError::Judge(cur_size + size))
				return false;
			void* cur_ptr = m_pContent.Load(MemoryOrder::Acquire);
			while (!m_Size.CompareExchangeWeak(cur_size, cur_size + size, MemoryOrder::AcquireRelease))
			{
				if (BufferOverflowError::Judge(cur_size + size))
					return false;
				cur_size = m_Size.Load(MemoryOrder::Acquire);
			}
			memcpy((Byte*)cur_ptr + cur_size, ptr, size);
			return true;
		}

	private:
		Atomic<void*> m_pContent;
		Atomic<SizeType> m_Size;
	};

}

/*!
@}
*/