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
#include "ForwardDefination.hpp"
#include "SGEString.h"
#include "Error.h"
#include <utility>

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/
	
	struct Uncopyable
	{
		Uncopyable() = default;
		Uncopyable(const Uncopyable&) = delete;
		Uncopyable(Uncopyable&&) = delete;
		Uncopyable& operator = (const Uncopyable&) = delete;
		Uncopyable& operator = (Uncopyable&&) = delete;
	};

	/*!
	@attention inherit `Singleton<T>` to use it,and define it as a friend in the class and 
	define the class's constructor as a private function at the same time.
	*/
	template<typename T>
	struct Singleton
	{
		/*!
		@todo use global variable
		*/
		inline static T& GetSingleton()
		{
			static T g_Instance;
			return g_Instance;
		}
	};

	template<typename T,typename U>
	struct Pair
	{
		Pair() = default;
		explicit Pair(const T& t,const U& u)
			:m_First(t),m_Second(u)
		{}
		explicit Pair(T&& t, U&& u)
			:m_First(t), m_Second(u)
		{}
		Pair(const Pair<T, U>& c)
			:Pair(c.m_First, c.m_Second)
		{}
		Pair(Pair<T,U>&& c)
			:Pair(std::move(c.m_First),std::move(c.m_Second))
		{}
		Pair<T, U>& operator = (const Pair<T, U>& c)
		{
			m_First = c.m_First;
			m_Second = c.m_Second;
			return *this;
		}
		Pair<T, U>& operator = (Pair<T, U>&& c)
		{
			m_First = std::move(c.m_First);
			m_Second = std::move(c.m_Second);
			return *this;
		}
		T m_First;
		U m_Second;
	};

	template<typename T>
	struct IsComparable
	{
	private:
		template<typename U>
		inline static constexpr std::enable_if_t<std::is_same_v<decltype(std::declval<U>() == std::declval<U>()), bool>, bool> Judge(int)
		{
			return true;
		}
		template<typename U>
		inline static constexpr bool Judge(...)
		{
			return false;
		}
	public:
		inline static constexpr bool Value = Judge<T>(0);
	};

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
			inline static const TChar sm_pContent[] = SGE_TSTR("The ControllableObject has been initialized");
			inline static bool Judge(void* pcontent)
			{
				return pcontent;
			}
		};
		struct HasNotBeenInitializedError
		{
			inline static const TChar sm_pContent[] = SGE_TSTR("The ControllableObject has not been initialized");
			inline static bool Judge(void* pcontent)
			{
				return !pcontent;
			}
		};
	public:
		inline ControllableObject()
			:m_pContent(nullptr)
		{}
		inline ~ControllableObject()
		{
			if(m_pContent)
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
		inline ControllableObject& operator =(const ControllableObject& obj)
		{
			SGE_ASSERT(HasNotBeenInitializedError, m_pContent);
			*m_pContent = obj.Get();
			return *this;
		}
		inline ControllableObject& operator =(ControllableObject&& obj)
		{
			SGE_ASSERT(HasNotBeenInitializedError, m_pContent);
			*m_pContent = std::move(obj.Get());
			return *this;
		}

		template<typename OtherAllocator>
		inline ControllableObject(const ControllableObject<T, OtherAllocator>& obj)
		{
			m_pContent = Allocator::template New<T>(obj.Get());
		}
		template<typename OtherAllocator>
		inline ControllableObject(ControllableObject<T, OtherAllocator>&& obj)
		{
			m_pContent = Allocator::template New<T>(std::move(obj.Get()));
		}
		template<typename OtherAllocator>
		inline ControllableObject& operator =(const ControllableObject<T, OtherAllocator>& obj)
		{
			SGE_ASSERT(HasNotBeenInitializedError, m_pContent);
			*m_pContent = obj.Get();
			return *this;
		}
		template<typename OtherAllocator>
		inline ControllableObject& operator =(ControllableObject<T, OtherAllocator>&& obj)
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
		inline ControllableObject& operator = (const T& obj)
		{
			SGE_ASSERT(HasNotBeenInitializedError, m_pContent);
			*m_pContent = obj;
			return *this;
		}
		inline ControllableObject& operator = (T&& obj)
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
		inline const T& Get()const
		{
			SGE_ASSERT(HasNotBeenInitializedError, m_pContent);
			return *m_pContent;
		}
		inline bool IsInitialized()const
		{
			return m_pContent;
		}

		template<typename OtherAllocator>
		inline bool operator == (const ControllableObject<T, OtherAllocator>& obj)const
		{
			if (IsInitialized() != obj.IsInitialized())
				return false;
			else if (IsInitialized())
				return *m_pContent == *obj.m_pContent;
			else
				return true;
		}
		inline bool operator == (const T& obj)const
		{
			if (IsInitialized())
				return *m_pContent == obj;
			else
				return false;
		}
	private:
		T* m_pContent;
	};

	/*!
	@}
	*/
}