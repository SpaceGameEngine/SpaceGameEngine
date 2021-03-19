/*
Copyright 2021 creatorlxd

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
#include "ContainerConcept.hpp"
#include "Error.h"

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/
	/*!
	@brief a adaptor for a bidirectional sequential iterator type to get its reverse iterator type.
	@note do not just use this type as the reverse iterator, you'd better make a new type derived
	from this type and add some useful functions according to the concrete container.
	@param T a bidirectional iterator type.
	*/
	template<typename IteratorType>
	class ReverseSequentialIterator
	{
	public:
		static_assert((IsBidirectionalSequentialIterator<IteratorType>::Value), "only the bidirectional iterator type can be passed to get the reverse iterator.");

		using ValueType = typename IteratorType::ValueType;

	public:
		inline ReverseSequentialIterator(const ReverseSequentialIterator& iter)
			: m_Content(iter)
		{
		}

		inline ReverseSequentialIterator& operator=(const ReverseSequentialIterator& iter)
		{
			SGE_ASSERT(SelfAssignmentError, this, &iter);
			m_Content = iter.m_Content;
			return *this;
		}

		inline ReverseSequentialIterator operator+(SizeType i) const
		{
			return ReverseSequentialIterator(m_Content - i);
		}

		inline ReverseSequentialIterator& operator+=(SizeType i)
		{
			m_Content -= i;
			return *this;
		}

		inline ReverseSequentialIterator& operator++()
		{
			m_Content -= 1;
			return *this;
		}

		inline const ReverseSequentialIterator operator++(int)
		{
			ReverseSequentialIterator re(*this);
			m_Content -= 1;
			return re;
		}

		inline ReverseSequentialIterator operator-(SizeType i) const
		{
			return ReverseSequentialIterator(m_Content + i);
		}

		inline ReverseSequentialIterator& operator-=(SizeType i)
		{
			m_Content += i;
			return *this;
		}

		inline ReverseSequentialIterator& operator--()
		{
			m_Content += 1;
			return *this;
		}

		inline const ReverseSequentialIterator operator--(int)
		{
			ReverseSequentialIterator re(*this);
			m_Content += 1;
			return re;
		}

		inline SizeType operator-(const ReverseSequentialIterator& iter) const
		{
			return iter.m_Content - m_Content;
		}

		inline ValueType* operator->() const
		{
			return m_Content.operator->();
		}

		inline ValueType& operator*() const
		{
			return m_Content.operator*();
		}

		inline bool operator==(const ReverseSequentialIterator& iter) const
		{
			return m_Content == iter.m_Content;
		}

		inline bool operator!=(const ReverseSequentialIterator& iter) const
		{
			return m_Content != iter.m_Content;
		}

		inline operator IteratorType() const
		{
			return m_Content;
		}

		inline ValueType* GetData()
		{
			return m_Content.GetData();
		}

		inline const ValueType* GetData() const
		{
			return m_Content.GetData();
		}

	protected:
		template<typename... Args>
		inline explicit ReverseSequentialIterator(Args&&... args)
			: m_Content(std::forward<Args>(args)...)
		{
		}

		template<typename... Args>
		inline ReverseSequentialIterator& operator=(Args&&... args)
		{
			m_Content.operator=(std::forward<Args>(args)...);
			return *this;
		}

	private:
		IteratorType m_Content;
	};
	/*!
	@}
	*/
}