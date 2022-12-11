/*
Copyright 2022 creatorlxd

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
#include "MemoryManager.h"

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{

	/*!
	@brief a wrapper of object which the user can control its lifetime.
	@note the ControllableObject can automatically release its resource if
	the user has not invoked its Release method.
	*/
	template<typename T, typename Allocator = DefaultAllocator>
	class ControllableObject
	{
	public:
		struct HasBeenInitializedError
		{
			inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The ControllableObject has been initialized");
			inline static bool Judge(void* pcontent)
			{
				return pcontent;
			}
		};
		struct HasNotBeenInitializedError
		{
			inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The ControllableObject has not been initialized");
			inline static bool Judge(void* pcontent)
			{
				return !pcontent;
			}
		};

		using ValueType = T;
		using AllocatorType = Allocator;

	public:
		template<typename _T, typename _Allocator>
		friend class ControllableObject;

		inline ControllableObject()
			: m_pContent(nullptr)
		{
		}
		inline ~ControllableObject()
		{
			if (m_pContent)
				Allocator::template Delete(m_pContent);
		}

		inline ControllableObject(const ControllableObject& obj)
		{
			m_pContent = Allocator::template New<T>(obj.Get());
		}
		inline ControllableObject(ControllableObject&& obj)
		{
			m_pContent = Allocator::template New<T>(std::move(obj.Get()));
		}
		inline ControllableObject& operator=(const ControllableObject& obj)
		{
			SGE_ASSERT(SelfAssignmentError, this, &obj);
			SGE_ASSERT(HasNotBeenInitializedError, m_pContent);
			*m_pContent = obj.Get();
			return *this;
		}
		inline ControllableObject& operator=(ControllableObject&& obj)
		{
			SGE_ASSERT(SelfAssignmentError, this, &obj);
			SGE_ASSERT(HasNotBeenInitializedError, m_pContent);
			*m_pContent = std::move(obj.Get());
			return *this;
		}

		template<typename U, typename OtherAllocator>
		inline ControllableObject(const ControllableObject<U, OtherAllocator>& obj)
		{
			m_pContent = Allocator::template New<T>(obj.Get());
		}
		template<typename U, typename OtherAllocator>
		inline ControllableObject(ControllableObject<U, OtherAllocator>&& obj)
		{
			m_pContent = Allocator::template New<T>(std::move(obj.Get()));
		}
		template<typename U, typename OtherAllocator>
		inline ControllableObject& operator=(const ControllableObject<U, OtherAllocator>& obj)
		{
			SGE_ASSERT(HasNotBeenInitializedError, m_pContent);
			*m_pContent = obj.Get();
			return *this;
		}
		template<typename U, typename OtherAllocator>
		inline ControllableObject& operator=(ControllableObject<U, OtherAllocator>&& obj)
		{
			SGE_ASSERT(HasNotBeenInitializedError, m_pContent);
			*m_pContent = std::move(obj.Get());
			return *this;
		}

		explicit inline ControllableObject(const T& obj)
		{
			m_pContent = Allocator::template New<T>(obj);
		}
		explicit inline ControllableObject(T&& obj)
		{
			m_pContent = Allocator::template New<T>(std::move(obj));
		}
		inline ControllableObject& operator=(const T& obj)
		{
			SGE_ASSERT(HasNotBeenInitializedError, m_pContent);
			*m_pContent = obj;
			return *this;
		}
		inline ControllableObject& operator=(T&& obj)
		{
			SGE_ASSERT(HasNotBeenInitializedError, m_pContent);
			*m_pContent = std::move(obj);
			return *this;
		}

		template<typename... Args>
		inline void Init(Args&&... args)
		{
			SGE_ASSERT(HasBeenInitializedError, m_pContent);
			m_pContent = Allocator::template New<T>(std::forward<Args>(args)...);
		}
		inline void Release()
		{
			SGE_ASSERT(HasNotBeenInitializedError, m_pContent);
			Allocator::template Delete(m_pContent);
			m_pContent = nullptr;
		}
		inline T& Get()
		{
			SGE_ASSERT(HasNotBeenInitializedError, m_pContent);
			return *m_pContent;
		}
		inline const T& Get() const
		{
			SGE_ASSERT(HasNotBeenInitializedError, m_pContent);
			return *m_pContent;
		}
		inline bool IsInitialized() const
		{
			return m_pContent;
		}

		template<typename U, typename OtherAllocator>
		inline bool operator==(const ControllableObject<U, OtherAllocator>& obj) const
		{
			if (IsInitialized() != obj.IsInitialized())
				return false;
			else if (IsInitialized())
				return *m_pContent == *obj.m_pContent;
			else
				return true;
		}

		inline bool operator==(const T& obj) const
		{
			if (IsInitialized())
				return *m_pContent == obj;
			else
				return false;
		}

		template<typename U, typename OtherAllocator>
		inline bool operator!=(const ControllableObject<U, OtherAllocator>& obj) const
		{
			if (IsInitialized() != obj.IsInitialized())
				return true;
			else if (IsInitialized())
				return *m_pContent != *obj.m_pContent;
			else
				return false;
		}

		inline bool operator!=(const T& obj) const
		{
			if (IsInitialized())
				return *m_pContent != obj;
			else
				return true;
		}

	private:
		T* m_pContent;
	};

}

/*!
@}
*/