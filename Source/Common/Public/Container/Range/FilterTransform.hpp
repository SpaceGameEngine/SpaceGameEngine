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
#include "ReverseTransform.hpp"

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/
	template<typename IteratorType, typename SentinelType>
	class FilterIterator;

	template<typename IteratorType, typename SentinelType = IteratorType, bool IsBidirectional = IsRangeBidirectionalIterator<IteratorType>::Value>
	class FilterIteratorExtensionForReverse
	{
	};

	template<typename IteratorType, typename SentinelType>
	class FilterIteratorExtensionForReverse<IteratorType, SentinelType, true>
	{
	private:
		/*!
		@brief Make the m_Iterator to be valid by checking with the filter function in the range information.
		@note If can not find the valid iterator, the m_Iterator will be the iterator which
		equals to the (begin iterator - 1) in the range information.
		*/
		inline void MakeValidBack()
		{
			while ((!reinterpret_cast<FilterIterator<IteratorType, SentinelType>*>(this)->m_pRangeInformation->m_FilterFunction(*(reinterpret_cast<FilterIterator<IteratorType, SentinelType>*>(this)->m_Iterator))) && ((reinterpret_cast<FilterIterator<IteratorType, SentinelType>*>(this)->m_pRangeInformation->m_Begin - 1) != (reinterpret_cast<FilterIterator<IteratorType, SentinelType>*>(this)->m_Iterator)))
				reinterpret_cast<FilterIterator<IteratorType, SentinelType>*>(this)->m_Iterator -= 1;
		}

	public:
		inline FilterIterator<IteratorType, SentinelType> operator-(SizeType size) const
		{
			FilterIterator<IteratorType, SentinelType> re(*reinterpret_cast<FilterIterator<IteratorType, SentinelType>*>(this));
			while (size--)
			{
				re.m_Iterator -= 1;
				re.MakeValidBack();
			}
			return re;
		}

		inline FilterIterator<IteratorType, SentinelType>& operator-=(SizeType size)
		{
			while (size--)
			{
				reinterpret_cast<FilterIterator<IteratorType, SentinelType>*>(this)->m_Iterator -= 1;
				MakeValidBack();
			}
			return *reinterpret_cast<FilterIterator<IteratorType, SentinelType>*>(this);
		}
	};

	template<typename IteratorType, typename SentinelType = IteratorType>
	class FilterIterator : public FilterIteratorExtensionForReverse<IteratorType, SentinelType>
	{
	public:
		static_assert((IsRangeIterator<IteratorType>::Value), "the IteratorType is not a RangeIterator");
		static_assert((IsRangeSentinel<SentinelType, IteratorType>::Value), "the SentinelType is not a RangeSentinel");

		friend class FilterIteratorExtensionForReverse<IteratorType, SentinelType, IsRangeBidirectionalIterator<IteratorType>::Value>;
		friend class FilterSentinel<IteratorType, SentinelType>;

		template<typename _IteratorType, typename _SentinelType, typename Allocator>
		friend Transform<FilterIterator<_IteratorType, _SentinelType>, FilterSentinel<_IteratorType, _SentinelType>> MakeFilterTransform(const Transform<_IteratorType, _SentinelType>& transform, const Function<bool(const typename _IteratorType::ValueType&)>& filter_func);

		template<typename _IteratorType, typename _SentinelType, typename Allocator>
		friend struct CustomReverseFunctor;

		friend class ReverseSentinel<FilterIterator<IteratorType, SentinelType>, FilterSentinel<IteratorType, SentinelType>>;

		using ValueType = typename IteratorType::ValueType;

		inline FilterIterator(const FilterIterator& iter)
			: m_Iterator(iter.m_Iterator), m_pRangeInformation(iter.m_pRangeInformation)
		{
			MakeValidForward();
		}

		inline FilterIterator& operator=(const FilterIterator& iter)
		{
			m_Iterator = iter.m_Iterator;
			m_pRangeInformation = iter.m_pRangeInformation;
			return *this;
		}

		inline FilterIterator operator+(SizeType size) const
		{
			FilterIterator re(*this);
			while (size--)
			{
				re.m_Iterator += 1;
				re.MakeValidForward();
			}
			return re;
		}

		inline FilterIterator& operator+=(SizeType size)
		{
			while (size--)
			{
				m_Iterator += 1;
				MakeValidForward();
			}
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

		inline bool operator==(const FilterIterator& iter) const
		{
			return m_Iterator == iter.m_Iterator && m_pRangeInformation == iter.m_pRangeInformation;
		}

		inline bool operator!=(const FilterIterator& iter) const
		{
			return m_Iterator != iter.m_Iterator || m_pRangeInformation != iter.m_pRangeInformation;
		}

	public:
		struct RangeInformation
		{
			IteratorType m_Begin;
			SentinelType m_End;
			Function<bool(const typename IteratorType::ValueType&)> m_FilterFunction;

			inline RangeInformation(const IteratorType& beg, const SentinelType& end, const Function<bool(const typename IteratorType::ValueType&)>& filter)
				: m_Begin(beg), m_End(end), m_FilterFunction(filter)
			{
			}
		};

	private:
		inline FilterIterator(const IteratorType& iter, RangeInformation& rinfo)
			: m_Iterator(iter), m_pRangeInformation(&rinfo)
		{
			MakeValidForward();
		}

		/*!
		@brief Make the m_Iterator to be valid by checking with the filter function in the range information.
		@note If can not find the valid iterator, the m_Iterator will be the iterator which
		equals to the end sentinel in the range information.
		*/
		inline void MakeValidForward()
		{
			while ((!m_pRangeInformation->m_FilterFunction(*m_Iterator)) && m_pRangeInformation->m_End != m_Iterator)
				m_Iterator += 1;
		}

	private:
		IteratorType m_Iterator;
		const RangeInformation* m_pRangeInformation;
	};

	template<typename IteratorType, typename SentinelType = IteratorType>
	class FilterSentinel
	{
	public:
		static_assert((IsRangeIterator<IteratorType>::Value), "the IteratorType is not a RangeIterator");
		static_assert((IsRangeSentinel<SentinelType, IteratorType>::Value), "the SentinelType is not a RangeSentinel");

		template<typename _IteratorType, typename _SentinelType, typename Allocator>
		friend Transform<FilterIterator<_IteratorType, _SentinelType>, FilterSentinel<_IteratorType, _SentinelType>> MakeFilterTransform(const Transform<_IteratorType, _SentinelType>& transform, const Function<bool(const typename _IteratorType::ValueType&)>& filter_func);

		inline bool operator!=(const FilterIterator<IteratorType, SentinelType>& iter) const
		{
			return m_pRangeInformation->m_End != iter.m_Iterator;
		}

	private:
		inline FilterSentinel(typename FilterIterator<IteratorType, SentinelType>::RangeInformation& range_info)
			: m_pRangeInformation(&range_info)
		{
		}

	private:
		const typename FilterIterator<IteratorType, SentinelType>::RangeInformation* m_pRangeInformation;
	};

	/*!
	@brief return the range which all elements can pass the filter function(return true).
	*/
	template<typename IteratorType, typename SentinelType = IteratorType, typename Allocator = DefaultAllocator>
	inline Transform<FilterIterator<IteratorType, SentinelType>, FilterSentinel<IteratorType, SentinelType>> MakeFilterTransform(const Transform<IteratorType, SentinelType>& transform, const Function<bool(const typename IteratorType::ValueType&)>& filter_func)
	{
		static_assert((IsRangeIterator<IteratorType>::Value), "the IteratorType is not a RangeIterator");
		static_assert((IsRangeSentinel<SentinelType, IteratorType>::Value), "the SentinelType is not a RangeSentinel");

		return Transform<FilterIterator<IteratorType, SentinelType>, FilterSentinel<IteratorType, SentinelType>>([=](AutoReleaseBuffer& arbuff) {
			auto range = transform.m_Function(arbuff);
			auto filter_range_info = arbuff.NewObject<typename FilterIterator<IteratorType, SentinelType>::RangeInformation, Allocator>(range.GetBegin(), range.GetEnd(), filter_func);
			return Range(FilterIterator<IteratorType, SentinelType>(range.GetBegin(), *filter_range_info), FilterSentinel<IteratorType, SentinelType>(*filter_range_info));
		});
	}

	template<typename T, typename Allocator = DefaultAllocator>
	class FilterTransform
	{
	public:
		using ValueType = T;

		template<typename IteratorType, typename SentinelType, typename _Allocator>
		friend auto operator|(const Transform<IteratorType, SentinelType>& transform, const FilterTransform<typename std::remove_cv_t<typename IteratorType::ValueType>, _Allocator>& filter_transform);

		inline FilterTransform(const Function<bool(const ValueType&)>& filter)
			: m_FilterFunction(filter)
		{
		}

	private:
		Function<bool(const ValueType&)> m_FilterFunction;
	};

	template<typename IteratorType, typename SentinelType = IteratorType, typename Allocator = DefaultAllocator>
	inline auto operator|(const Transform<IteratorType, SentinelType>& transform, const FilterTransform<typename std::remove_cv_t<typename IteratorType::ValueType>, Allocator>& filter_transform)
	{
		return MakeFilterTransform(transform, filter_transform.m_FilterFunction);
	}

	template<typename IteratorType, typename SentinelType>
	class ReverseSentinel<FilterIterator<IteratorType, SentinelType>, FilterSentinel<IteratorType, SentinelType>>
	{
	public:
		static_assert((IsRangeBidirectionalIterator<IteratorType>::Value), "the IteratorType is not a Bidirectional RangeIterator");
		static_assert((IsRangeSentinel<SentinelType, IteratorType>::Value), "the SentinelType is not a RangeSentinel");

		template<typename _IteratorType, typename _SentinelType, typename Allocator>
		friend struct CustomReverseFunctor;

		inline bool operator!=(const ReverseIterator<FilterIterator<IteratorType, SentinelType>, FilterSentinel<IteratorType, SentinelType>>& iter) const
		{
			return m_pRangeInformation->m_Begin - 1 != iter.m_Iterator.m_Iterator;
		}

	private:
		inline ReverseSentinel<FilterIterator<IteratorType, SentinelType>, FilterSentinel<IteratorType, SentinelType>>(typename FilterIterator<IteratorType, SentinelType>::RangeInformation& range_info)
			: m_pRangeInformation(&range_info)
		{
		}

	private:
		const typename FilterIterator<IteratorType, SentinelType>::RangeInformation* m_pRangeInformation;
	};

	template<typename IteratorType, typename SentinelType, typename Allocator>
	struct CustomReverseFunctor<FilterIterator<IteratorType, SentinelType>, FilterSentinel<IteratorType, SentinelType>, Allocator>
	{
		inline Transform<ReverseIterator<FilterIterator<IteratorType, SentinelType>, FilterSentinel<IteratorType, SentinelType>>, ReverseSentinel<FilterIterator<IteratorType, SentinelType>, FilterSentinel<IteratorType, SentinelType>>> operator()(const Transform<FilterIterator<IteratorType, SentinelType>, FilterSentinel<IteratorType, SentinelType>>& transform) const
		{
			static_assert((IsRangeBidirectionalIterator<IteratorType>::Value), "the IteratorType is not a Bidirectional RangeIterator");
			static_assert((IsRangeSentinel<SentinelType, IteratorType>::Value), "the SentinelType is not a RangeSentinel");

			return Transform<ReverseIterator<FilterIterator<IteratorType, SentinelType>, FilterSentinel<IteratorType, SentinelType>>, ReverseSentinel<FilterIterator<IteratorType, SentinelType>, FilterSentinel<IteratorType, SentinelType>>>([=](AutoReleaseBuffer& arbuff) {
				auto range = transform.m_Function(arbuff);
				typename FilterIterator<IteratorType, SentinelType>::RangeInformation* prange_info = const_cast<typename FilterIterator<IteratorType, SentinelType>::RangeInformation*>(range.GetBegin().m_pRangeInformation);
				return Range(ReverseIterator<FilterIterator<IteratorType, SentinelType>, FilterSentinel<IteratorType, SentinelType>>(FilterIterator<IteratorType, SentinelType>(prange_info->m_End - 1, *prange_info)), ReverseSentinel<FilterIterator<IteratorType, SentinelType>, FilterSentinel<IteratorType, SentinelType>>(*prange_info));
			});
		}
	};

	/*!
	@}
	*/
}