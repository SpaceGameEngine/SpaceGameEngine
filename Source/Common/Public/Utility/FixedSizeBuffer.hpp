/*
Copyright 2022 creatorlxd

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

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{

	template<SizeType MaxSize, typename Allocator = DefaultAllocator>
	class FixedSizeBuffer
	{
	public:
		inline static constexpr const SizeType sm_MaxSize = MaxSize;

		struct BufferOverflowError
		{
			inline static const TChar sm_pContent[] = SGE_TSTR("The buffer is overflow.");
			inline static bool Judge(SizeType size)
			{
				return size > MaxSize;
			}
		};

		template<SizeType _MaxSize, typename _Allocator>
		friend class FixedSizeBuffer;

		inline FixedSizeBuffer()
			: m_pContent(Allocator::RawNew(MaxSize)), m_Size(0)
		{
		}

		inline ~FixedSizeBuffer()
		{
			if (m_pContent)
				Allocator::RawDelete(m_pContent);
		}

		inline FixedSizeBuffer(const FixedSizeBuffer& buffer)
			: m_pContent(Allocator::RawNew(MaxSize))
		{
			memcpy(m_pContent, buffer.m_pContent, buffer.m_Size);
			m_Size = buffer.m_Size;
		}

		inline FixedSizeBuffer(FixedSizeBuffer&& buffer)
			: m_pContent(buffer.m_pContent), m_Size(buffer.m_Size)
		{
			buffer.m_pContent = nullptr;
		}

		inline FixedSizeBuffer& operator=(const FixedSizeBuffer& buffer)
		{
			SGE_ASSERT(SelfAssignmentError, this, &buffer);
			memcpy(m_pContent, buffer.m_pContent, buffer.m_Size);
			m_Size = buffer.m_Size;
			return *this;
		}

		inline FixedSizeBuffer& operator=(FixedSizeBuffer&& buffer)
		{
			SGE_ASSERT(SelfAssignmentError, this, &buffer);
			m_pContent = buffer.m_pContent;
			m_Size = buffer.m_Size;
			buffer.m_pContent = nullptr;
			return *this;
		}

		template<typename OtherAllocator>
		inline FixedSizeBuffer(const FixedSizeBuffer<MaxSize, OtherAllocator>& buffer)
			: m_pContent(Allocator::RawNew(MaxSize))
		{
			memcpy(m_pContent, buffer.m_pContent, buffer.m_Size);
			m_Size = buffer.m_Size;
		}

		template<typename OtherAllocator>
		inline FixedSizeBuffer(FixedSizeBuffer<MaxSize, OtherAllocator>&& buffer)
			: m_pContent(Allocator::RawNew(MaxSize))
		{
			memcpy(m_pContent, buffer.m_pContent, buffer.m_Size);
			m_Size = buffer.m_Size;
		}

		template<typename OtherAllocator>
		inline FixedSizeBuffer& operator=(const FixedSizeBuffer<MaxSize, OtherAllocator>& buffer)
		{
			memcpy(m_pContent, buffer.m_pContent, buffer.m_Size);
			m_Size = buffer.m_Size;
			return *this;
		}

		template<typename OtherAllocator>
		inline FixedSizeBuffer& operator=(FixedSizeBuffer<MaxSize, OtherAllocator>&& buffer)
		{
			memcpy(m_pContent, buffer.m_pContent, buffer.m_Size);
			m_Size = buffer.m_Size;
			return *this;
		}

		inline void* GetData()
		{
			return m_pContent;
		}

		inline const void* GetData() const
		{
			return m_pContent;
		}

		inline void SetSize(SizeType size)
		{
			SGE_ASSERT(BufferOverflowError, size);
			m_Size = size;
		}

		inline SizeType GetSize() const
		{
			return m_Size;
		}

		inline SizeType GetFreeSize() const
		{
			SGE_ASSERT(BufferOverflowError, m_Size);
			return MaxSize - m_Size;
		}

		inline void Clear()
		{
			m_Size = 0;
		}

		inline void Append(const void* ptr, SizeType size)
		{
			SGE_ASSERT(NullPointerError, ptr);
			SGE_ASSERT(BufferOverflowError, m_Size + size);
			memcpy((Byte*)m_pContent + m_Size, ptr, size);
			m_Size += size;
		}

	private:
		void* m_pContent;
		SizeType m_Size;
	};

}

/*!
@}
*/