/*
Copyright 2019 creatorlxd

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
#include "TransformCore.hpp"

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/
	template<typename IteratorType, typename SentinelType>
	class ReverseSentinel;

	template<typename IteratorType, typename SentinelType = IteratorType>
	class ReverseIterator
	{
	public:
		static_assert((IsRangeBidirectionalIterator<IteratorType>::Result), "the IteratorType is not a Bidirectional RangeIterator");
		static_assert((IsRangeSentinel<SentinelType, IteratorType>::Result), "the SentinelType is not a RangeSentinel");

		using ValueType = typename IteratorType::ValueType;

		friend class ReverseSentinel<IteratorType, SentinelType>;
		template<typename _IteratorType, typename _SentinelType, typename Allocator>
		friend Transform<ReverseIterator<_IteratorType, _SentinelType>, ReverseSentinel<_IteratorType, _SentinelType>> MakeReverseTransform(const Transform<_IteratorType, _SentinelType>& transform);

		inline ReverseIterator(const ReverseIterator& iter)
			: m_Iterator(iter.m_Iterator)
		{
		}

		inline ReverseIterator& operator=(const ReverseIterator& iter)
		{
			m_Iterator = iter.m_Iterator;
			return *this;
		}

		inline ReverseIterator operator+(SizeType i) const
		{
			return ReverseIterator(m_Iterator - i);
		}

		inline ReverseIterator& operator+=(SizeType i)
		{
			m_Iterator -= i;
			return *this;
		}

		inline ReverseIterator operator-(SizeType i) const
		{
			return ReverseIterator(m_Iterator + i);
		}

		inline ReverseIterator& operator-=(SizeType i)
		{
			m_Iterator += i;
			return *this;
		}

		inline ValueType* operator->() const
		{
			return m_Iterator.operator->();
		}

		inline ValueType& operator*() const
		{
			return m_Iterator.operator*();
		}

		inline bool operator==(const ReverseIterator& iter) const
		{
			return m_Iterator == iter.m_Iterator;
		}

		inline bool operator!=(const ReverseIterator& iter) const
		{
			return m_Iterator != iter.m_Iterator;
		}

		inline operator IteratorType() const
		{
			return m_Iterator;
		}

	private:
		inline ReverseIterator(const IteratorType& iter)
			: m_Iterator(iter)
		{
		}

	private:
		IteratorType m_Iterator;
	};

	template<typename IteratorType, typename SentinelType = IteratorType>
	class ReverseSentinel
	{
	public:
		static_assert((IsRangeBidirectionalIterator<IteratorType>::Result), "the IteratorType is not a Bidirectional RangeIterator");
		static_assert((IsRangeSentinel<SentinelType, IteratorType>::Result), "the SentinelType is not a RangeSentinel");

		template<typename _IteratorType, typename _SentinelType, typename Allocator>
		friend Transform<ReverseIterator<_IteratorType, _SentinelType>, ReverseSentinel<_IteratorType, _SentinelType>> MakeReverseTransform(const Transform<_IteratorType, _SentinelType>& transform);

		inline bool operator!=(const ReverseIterator<IteratorType, SentinelType>& iter) const
		{
			return m_Sentinel != static_cast<IteratorType>(iter);
		}

	private:
		inline ReverseSentinel(const SentinelType& sentinel)
			: m_Sentinel(sentinel)
		{
		}

	private:
		SentinelType m_Sentinel;
	};

	/*!
	@brief return the range which all elements have been reversed.
	@note The function's default implement suit the situation which the SentinelType equals
	to the IteratorType. If you want it to support iterators which do not suit the condition 
	above, you need to specify this function.
	*/
	template<typename IteratorType, typename SentinelType = IteratorType, typename Allocator = DefaultAllocator>
	inline Transform<ReverseIterator<IteratorType, SentinelType>, ReverseSentinel<IteratorType, SentinelType>> MakeReverseTransform(const Transform<IteratorType, SentinelType>& transform)
	{
		static_assert((IsRangeBidirectionalIterator<IteratorType>::Result), "the IteratorType is not a Bidirectional RangeIterator");
		static_assert((IsRangeSentinel<SentinelType, IteratorType>::Result), "the SentinelType is not a RangeSentinel");

		static_assert((std::is_same_v<IteratorType, SentinelType>), "default implement does not suit this condition, IteratorType need to be same with the SentinelType.");
		return Transform<ReverseIterator<IteratorType, SentinelType>, ReverseSentinel<IteratorType, SentinelType>>([=](AutoReleaseBuffer& arbuff) {
			auto range = transform.m_Function(arbuff);
			return Range(ReverseIterator<IteratorType, SentinelType>(range.GetEnd() - 1), ReverseSentinel<IteratorType, SentinelType>(range.GetBegin() - 1));
		});
	}

	template<typename Allocator = DefaultAllocator>
	class ReverseTransform
	{
	};

	template<typename IteratorType, typename SentinelType = IteratorType, typename Allocator = DefaultAllocator>
	inline auto operator|(const Transform<IteratorType, SentinelType>& transform, const ReverseTransform<Allocator>& reverse_transform)
	{
		return MakeReverseTransform<IteratorType, SentinelType, Allocator>(transform);
	}
	/*!
	@}
	*/
}