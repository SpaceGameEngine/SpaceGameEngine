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
#include "../ContainerConcept.hpp"

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/
	/*!
	@brief a generic object that represents a sequence.
	*/
	template<typename IteratorType, typename SentinelType = IteratorType>
	class Range
	{
	public:
		static_assert((IsRangeIterator<IteratorType>::Result), "the IteratorType is not a RangeIterator");
		static_assert((IsRangeSentinel<SentinelType, IteratorType>::Result), "the SentinelType is not a RangeSentinel");

		using ValueType = typename IteratorType::ValueType;
		using BeginIteratorType = IteratorType;
		using EndIteratorType = SentinelType;

		inline Range(const IteratorType& b, const SentinelType& e)
			: m_BeginIterator(b), m_EndIterator(e)
		{
		}

		inline IteratorType GetBegin() const
		{
			return m_BeginIterator;
		}

		inline SentinelType GetEnd() const
		{
			return m_EndIterator;
		}

	private:
		IteratorType m_BeginIterator;
		SentinelType m_EndIterator;
	};
	/*!
	@}
	*/
}