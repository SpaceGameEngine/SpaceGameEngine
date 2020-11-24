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
#include "Container/Range/TransformCore.hpp"
#include "Container/Range/InfiniteSentinel.hpp"

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

	template<typename T, typename U = T>
	class SequenceIterator
	{
	public:
		using ValueType = T;

		template<typename T2, typename U2>
		friend Range<SequenceIterator<T2, U2>, InfiniteSentinel> MakeInfiniteSequence(const T2& start, const U2& step);

		template<typename T2, typename U2>
		friend Range<SequenceIterator<T2, U2>, SequenceIterator<T2, U2>> MakeSequence(const T2& start, const U2& step, const T2& end);

		inline SequenceIterator(const SequenceIterator& iter)
			: m_Content(iter.m_Content),
			  m_Increment(iter.m_Increment)
		{
		}

		inline SequenceIterator& operator=(const SequenceIterator& iter)
		{
			m_Content = iter.m_Content;
			m_Increment = iter.m_Increment;
			return *this;
		}

		inline SequenceIterator operator+(SizeType size) const
		{
			SequenceIterator re(*this);
			for (SizeType i = 0; i < size; i++)
				re.m_Content += re.m_Increment;
			return re;
		}

		inline SequenceIterator& operator+=(SizeType size)
		{
			for (SizeType i = 0; i < size; i++)
				m_Content += m_Increment;
			return *this;
		}

		inline T* operator->()
		{
			return &m_Content;
		}

		inline T& operator*()
		{
			return m_Content;
		}

		inline const T* operator->() const
		{
			return &m_Content;
		}

		inline const T& operator*() const
		{
			return m_Content;
		}

		inline bool operator==(const SequenceIterator& iter) const
		{
			return m_Content == iter.m_Content && m_Increment == iter.m_Increment;
		}

		inline bool operator!=(const SequenceIterator& iter) const
		{
			return m_Content != iter.m_Content || m_Increment != iter.m_Increment;
		}

	private:
		inline SequenceIterator(const T& content, const U& increment)
			: m_Content(content), m_Increment(increment)
		{
		}

	private:
		T m_Content;
		U m_Increment;
	};

	template<typename T, typename U = T>
	inline Range<SequenceIterator<T, U>, InfiniteSentinel> MakeInfiniteSequence(const T& start, const U& step)
	{
		return Range(SequenceIterator(start, step), InfiniteSentinel());
	}

	template<typename T, typename U = T>
	inline Range<SequenceIterator<T, U>, SequenceIterator<T, U>> MakeSequence(const T& start, const U& step, const T& end)
	{
		return Range(SequenceIterator(start, step), SequenceIterator(end, step));
	}
	/*!
	@}
	*/
}