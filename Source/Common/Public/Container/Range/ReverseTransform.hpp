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
	@}
	*/
}