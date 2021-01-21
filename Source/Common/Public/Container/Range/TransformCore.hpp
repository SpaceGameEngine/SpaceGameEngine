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
#include "RangeCore.hpp"
#include "Utility/AutoReleaseBuffer.h"

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

	template<typename IteratorType, typename SentinelType>
	class FilterIterator;

	template<typename IteratorType, typename SentinelType>
	class FilterSentinel;

	template<typename IteratorType, typename SentinelType>
	class ReverseIterator;

	template<typename IteratorType, typename SentinelType>
	class ReverseSentinel;

	/*!
	@brief a generic object that represents a transform between the ranges.
	*/
	template<typename IteratorType, typename SentinelType = IteratorType>
	class Transform
	{
	public:
		static_assert((IsRangeIterator<IteratorType>::Result), "the IteratorType is not a RangeIterator");
		static_assert((IsRangeSentinel<SentinelType, IteratorType>::Result), "the SentinelType is not a RangeSentinel");

		using ValueType = typename IteratorType::ValueType;
		using BeginIteratorType = IteratorType;
		using EndIteratorType = SentinelType;
		using RangeType = Range<IteratorType, SentinelType>;

		template<typename _IteratorType, typename _SentinelType, typename Allocator>
		friend Vector<typename std::remove_cv_t<typename _IteratorType::ValueType>, Allocator> CastToVector(const Transform<_IteratorType, _SentinelType>& transform);

		template<typename _IteratorType, typename _SentinelType, typename Allocator>
		friend Transform<FilterIterator<_IteratorType, _SentinelType>, FilterSentinel<_IteratorType, _SentinelType>> MakeFilterTransform(const Transform<_IteratorType, _SentinelType>& transform, const Function<bool(const typename _IteratorType::ValueType&)>& filter_func);

		template<typename _IteratorType, typename _SentinelType, typename Allocator>
		friend Transform<typename Vector<std::remove_cv_t<typename _IteratorType::ValueType>, Allocator>::Iterator, typename Vector<std::remove_cv_t<typename _IteratorType::ValueType>, Allocator>::Iterator> MakeTakeTransform(const Transform<_IteratorType, _SentinelType>& transform, SizeType n);

		template<typename _IteratorType, typename _SentinelType, typename Allocator>
		friend Transform<ReverseIterator<_IteratorType, _SentinelType>, ReverseSentinel<_IteratorType, _SentinelType>> MakeReverseTransform(const Transform<_IteratorType, _SentinelType>& transform);

		template<typename _IteratorType, typename _SentinelType, typename Allocator>
		friend struct CustomReverseFunctor;

		explicit inline Transform(const Function<RangeType(AutoReleaseBuffer&)>& func)
			: m_Function(func)
		{
		}

		inline Transform(const RangeType& range)
			: m_Function([range](AutoReleaseBuffer&) { return range; })
		{
		}

	private:
		Function<RangeType(AutoReleaseBuffer&)> m_Function;
	};

	/*!
	@}
	*/
}