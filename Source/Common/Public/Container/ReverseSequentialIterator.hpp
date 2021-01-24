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
	template<typename T>
	class ReverseSequentialIterator
	{
	};

	template<template<typename> class IteratorImpl, typename T>
	class ReverseSequentialIterator<IteratorImpl<T>>
	{
	public:
		static_assert((IsBidirectionalSequentialIterator<IteratorImpl<T>>::Value), "only the bidirectional iterator type can be passed to get the reverse iterator.");

		using ValueType = T;

	public:
		inline ReverseSequentialIterator(const ReverseSequentialIterator& iter)
			: m_Content(iter)
		{
		}

		inline ReverseSequentialIterator& operator=(const ReverseSequentialIterator& iter)
		{
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

		inline ReverseSequentialIterator operator-(SizeType i) const
		{
			return ReverseSequentialIterator(m_Content + i);
		}

		inline ReverseSequentialIterator& operator-=(SizeType i)
		{
			m_Content += i;
			return *this;
		}

		inline SizeType operator-(const ReverseSequentialIterator& iter) const
		{
			return iter.m_Content - m_Content;
		}

		inline T* operator->() const
		{
			return m_Content.operator->();
		}

		inline T& operator*() const
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

		inline operator IteratorImpl<T>() const
		{
			return m_Content;
		}

	protected:
		template<typename... Args>
		inline explicit ReverseSequentialIterator(Args&&... args)
			: m_Content(std::forward<Args>(args)...)
		{
		}

	private:
		IteratorImpl<T> m_Content;
	};
	/*!
	@}
	*/
}