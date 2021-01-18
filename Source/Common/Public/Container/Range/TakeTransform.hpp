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
#include "Container/Range/TransformCore.hpp"

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

	/*!
	@brief return the range which takes n elements from the begin of the input range.
	*/
	template<typename IteratorType, typename SentinelType = IteratorType, typename Allocator = DefaultAllocator>
	inline Transform<typename Vector<std::remove_cv_t<typename IteratorType::ValueType>, Allocator>::Iterator, typename Vector<std::remove_cv_t<typename IteratorType::ValueType>, Allocator>::Iterator> MakeTakeTransform(const Transform<IteratorType, SentinelType>& transform, SizeType n)
	{
		static_assert((IsRangeIterator<IteratorType>::Result), "the IteratorType is not a RangeIterator");
		static_assert((IsRangeSentinel<SentinelType, IteratorType>::Result), "the SentinelType is not a RangeSentinel");

		return Transform<typename Vector<std::remove_cv_t<typename IteratorType::ValueType>, Allocator>::Iterator, typename Vector<std::remove_cv_t<typename IteratorType::ValueType>, Allocator>::Iterator>([=](AutoReleaseBuffer& arbuff) {
			auto pvec = arbuff.NewObject<Vector<std::remove_cv_t<typename IteratorType::ValueType>, Allocator>, Allocator>();
			auto range = transform.m_Function(arbuff);
			SizeType cnt = 0;
			for (auto iter = range.GetBegin(); range.GetEnd() != iter && cnt < n; iter += 1, cnt += 1)
				pvec->EmplaceBack(*iter);
			return Range(pvec->GetBegin(), pvec->GetEnd());
		});
	}

	template<typename Allocator = DefaultAllocator>
	class TakeTransform
	{
	public:
		template<typename IteratorType, typename SentinelType, typename _Allocator>
		friend inline auto operator|(const Transform<IteratorType, SentinelType>& transform, const TakeTransform<_Allocator>& take_transform);

		inline TakeTransform(SizeType size)
			: m_Size(size)
		{
		}

	private:
		SizeType m_Size;
	};

	template<typename IteratorType, typename SentinelType = IteratorType, typename Allocator = DefaultAllocator>
	inline auto operator|(const Transform<IteratorType, SentinelType>& transform, const TakeTransform<Allocator>& take_transform)
	{
		return MakeTakeTransform(transform, take_transform.m_Size);
	}

	/*!
	@}
	*/
}