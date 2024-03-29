﻿/*
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
#include "Error.h"
#include "MemoryManager.h"
#include "MetaData/MetaObject.hpp"
#include "Utility/ControllableObject.hpp"
#include <type_traits>
#include <functional>

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

	/*!
	@brief judge a type is a correct funtion type or not.
	@note use SFINAE and magical template matching.
	*/
	template<typename T, typename Func>
	struct IsCorrectFunction
	{
		inline static constexpr const bool Value = false;
	};

	template<typename Ret, typename... Args>
	struct IsCorrectFunction<Ret (*)(Args...), Ret(Args...)>
	{
		inline static constexpr const bool Value = true;
	};

	template<typename Ret, typename Class, typename... Args>
	struct IsCorrectFunction<Ret (Class::*)(Args...), Ret(Class*, Args...)>
	{
		inline static constexpr const bool Value = true;
	};

	template<typename Ret, typename Class, typename... Args>
	struct IsCorrectFunction<Ret (Class::*)(Args...) const, Ret(const Class*, Args...)>
	{
		inline static constexpr const bool Value = true;
	};

	template<typename T, typename Ret, typename... Args>
	struct IsCorrectFunction<T, Ret(Args...)>
	{
	private:
		template<typename _T, typename _Ret, typename... _Args>
		inline static constexpr std::enable_if_t<std::is_same_v<decltype(std::declval<_T>()(std::declval<_Args>()...)), _Ret>, bool> Judge(int)
		{
			return true;
		}

		template<typename _T, typename _Ret, typename... _Args>
		inline static constexpr bool Judge(...)
		{
			return false;
		}

	public:
		inline static constexpr const bool Value = Judge<T, Ret, Args...>(0);
	};

	/*!
	@brief a callable object wrapper.
	*/
	template<typename T, typename Allocator = DefaultAllocator>
	class Function
	{
	};

	template<typename Allocator, typename Ret, typename... Args>
	class Function<Ret(Args...), Allocator>
	{
	public:
		template<typename T>
		struct IsFunction
		{
			inline static constexpr const bool Value = false;
		};

		template<typename _Allocator, typename _Ret, typename... _Args>
		struct IsFunction<Function<_Ret(_Args...), _Allocator>>
		{
			inline static constexpr const bool Value = true;
		};

		using AllocatorType = Allocator;
		using FunctionType = Ret(Args...);

	public:
		template<typename _T, typename _Allocator>
		friend class Function;

		inline Function() = delete;

		inline ~Function()
		{
		}

		inline Function(const Function& func)
			: m_pInvoke(func.m_pInvoke), m_Content(func.m_Content)
		{
		}

		inline Function(Function&& func)
			: m_pInvoke(func.m_pInvoke), m_Content(std::move(func.m_Content))
		{
		}

		inline Function& operator=(const Function& func)
		{
			SGE_ASSERT(SelfAssignmentError, this, &func);
			if (m_Content.Get().GetMetaData() == func.m_Content.Get().GetMetaData())
				m_Content = func.m_Content;
			else
			{
				m_pInvoke = func.m_pInvoke;
				m_Content.Release();
				m_Content.Init(func.m_Content.Get());
			}
			return *this;
		}

		inline Function& operator=(Function&& func)
		{
			SGE_ASSERT(SelfAssignmentError, this, &func);
			if (m_Content.Get().GetMetaData() == func.m_Content.Get().GetMetaData())
				m_Content = std::move(func.m_Content);
			else
			{
				m_pInvoke = func.m_pInvoke;
				m_Content.Release();
				m_Content.Init(std::move(func.m_Content.Get()));
			}
			return *this;
		}

		template<typename OtherAllocator>
		inline Function(const Function<Ret(Args...), OtherAllocator>& func)
			: m_pInvoke((decltype(m_pInvoke))func.m_pInvoke), m_Content(func.m_Content)
		{
		}

		template<typename OtherAllocator>
		inline Function(Function<Ret(Args...), OtherAllocator>&& func)
			: m_pInvoke((decltype(m_pInvoke))func.m_pInvoke), m_Content(std::move(func.m_Content))
		{
		}

		template<typename OtherAllocator>
		inline Function& operator=(const Function<Ret(Args...), OtherAllocator>& func)
		{
			if (m_Content.Get().GetMetaData() == func.m_Content.Get().GetMetaData())
				m_Content = func.m_Content;
			else
			{
				m_pInvoke = (decltype(m_pInvoke))func.m_pInvoke;
				m_Content.Release();
				m_Content.Init(func.m_Content.Get());
			}
			return *this;
		}

		template<typename OtherAllocator>
		inline Function& operator=(Function<Ret(Args...), OtherAllocator>&& func)
		{
			if (m_Content.Get().GetMetaData() == func.m_Content.Get().GetMetaData())
				m_Content = std::move(func.m_Content);
			else
			{
				m_pInvoke = (decltype(m_pInvoke))func.m_pInvoke;
				m_Content.Release();
				m_Content.Init(std::move(func.m_Content.Get()));
			}
			return *this;
		}

		template<typename T, typename = std::enable_if_t<IsFunction<std::decay_t<T>>::Value == false, bool>>
		inline Function(T&& func)
		{
			static_assert(IsCorrectFunction<std::decay_t<T>, Ret(Args...)>::Value, "Function can only be constructed by callable object");
			m_pInvoke = [](const MetaObject<Allocator>& obj, Args... args) -> Ret {
				return std::invoke((std::decay_t<T>)(obj.template Get<std::decay_t<T>>()), static_cast<Args>(args)...);
			};
			m_Content.Init(SpaceGameEngine::GetMetaData<std::decay_t<T>>(), std::forward<T>(func));
		}

		template<typename T,
				 typename = std::enable_if_t<IsFunction<std::decay_t<T>>::Value == false, bool>,
				 typename = std::enable_if_t<std::is_same_v<std::decay_t<decltype(std::declval<ControllableObject<MetaObject<Allocator>, Allocator>>() = std::forward<T>(std::declval<T&&>()))>, ControllableObject<MetaObject<Allocator>, Allocator>>, bool>>
		inline Function& operator=(T&& func)
		{
			static_assert(IsCorrectFunction<std::decay_t<T>, Ret(Args...)>::Value, "Function can only be constructed by callable object");
			if (SpaceGameEngine::GetMetaData<std::decay_t<T>>() == m_Content.Get().GetMetaData())
				m_Content = std::forward<T>(func);
			else
			{
				m_pInvoke = [](const MetaObject<Allocator>& obj, Args... args) -> Ret {
					return std::invoke((std::decay_t<T>)(obj.template Get<std::decay_t<T>>()), static_cast<Args>(args)...);
				};
				m_Content.Release();
				m_Content.Init(SpaceGameEngine::GetMetaData<std::decay_t<T>>(), std::forward<T>(func));
			}
			return *this;
		}

		template<typename T, typename = std::enable_if_t<IsFunction<std::decay_t<T>>::Value == false, bool>>
		inline Function& operator=(T&& func)
		{
			static_assert(IsCorrectFunction<std::decay_t<T>, Ret(Args...)>::Value, "Function can only be constructed by callable object");
			m_pInvoke = [](const MetaObject<Allocator>& obj, Args... args) -> Ret {
				return std::invoke((std::decay_t<T>)(obj.template Get<std::decay_t<T>>()), static_cast<Args>(args)...);
			};
			m_Content.Release();
			m_Content.Init(SpaceGameEngine::GetMetaData<std::decay_t<T>>(), std::forward<T>(func));
			return *this;
		}

		template<typename T>
		inline T& Get()
		{
			return m_Content.Get().template Get<T>();
		}

		template<typename T>
		inline const T& Get() const
		{
			return m_Content.Get().template Get<T>();
		}

		inline const MetaData& GetMetaData() const
		{
			return m_Content.Get().GetMetaData();
		}

		inline Ret operator()(Args... args) const
		{
			return m_pInvoke(m_Content.Get(), static_cast<Args>(args)...);
		}

		template<typename OtherAllocator>
		inline bool operator==(const Function<Ret(Args...), OtherAllocator>& func) const
		{
			return m_Content == func.m_Content;
		}

	private:
		Ret (*m_pInvoke)(const MetaObject<Allocator>&, Args...);
		ControllableObject<MetaObject<Allocator>, Allocator> m_Content;
	};

	/*!
	@file
	@todo add bind and other functional support.
	*/

	/*!
	@}
	*/
}