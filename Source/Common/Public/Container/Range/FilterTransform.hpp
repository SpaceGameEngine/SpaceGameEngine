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
#include "Container/ContainerConcept.hpp"
#include "Container/Vector.hpp"
#include "MemoryManager.h"
#include "TransformCore.hpp"
#include "Function.hpp"

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

	/*!
	@brief return the range which all elements can pass the filter function(return true).
	*/
	template<typename IteratorType, typename SentinelType = IteratorType, typename Allocator = DefaultAllocator>
	inline Transform<typename Vector<typename IteratorType::ValueType, Allocator>::Iterator, typename Vector<typename IteratorType::ValueType, Allocator>::Iterator> MakeFilterTransform(const Transform<IteratorType, SentinelType>& transform, const Function<bool(const typename IteratorType::ValueType&)>& filter_func)
	{
		static_assert((IsRangeIterator<IteratorType>::Result), "the IteratorType is not a RangeIterator");
		static_assert((IsRangeSentinel<SentinelType, IteratorType>::Result), "the SentinelType is not a RangeSentinel");

		return Transform<typename Vector<typename IteratorType::ValueType, Allocator>::Iterator, typename Vector<typename IteratorType::ValueType, Allocator>::Iterator>([=](AutoReleaseBuffer& arbuff) {
			auto pvec = arbuff.NewObject<Vector<typename IteratorType::ValueType, Allocator>, Allocator>();
			auto range = transform.m_Function(arbuff);
			for (auto iter = range.GetBegin(); range.GetEnd() != iter; iter += 1)
				if (filter_func(*iter) == true)
					pvec->EmplaceBack(*iter);
			return Range(pvec->GetBegin(), pvec->GetEnd());
		});
	}

	template<typename T, typename Allocator = DefaultAllocator>
	class FilterTransform
	{
	public:
		using ValueType = T;

		template<typename IteratorType, typename SentinelType, typename _Allocator>
		friend inline auto operator|(const Transform<IteratorType, SentinelType>& transform, const FilterTransform<typename IteratorType::ValueType, _Allocator>& filter_transform);

		inline FilterTransform(const Function<bool(const ValueType&)>& filter)
			: m_FilterFunction(filter)
		{
		}

	private:
		Function<bool(const ValueType&)> m_FilterFunction;
	};

	template<typename IteratorType, typename SentinelType = IteratorType, typename Allocator = DefaultAllocator>
	inline auto operator|(const Transform<IteratorType, SentinelType>& transform, const FilterTransform<typename IteratorType::ValueType, Allocator>& filter_transform)
	{
		return MakeFilterTransform(transform, filter_transform.m_FilterFunction);
	}
	/*!
	@}
	*/
}