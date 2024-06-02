/*
Copyright 2024 creatorlxd

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

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{

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

	namespace FunctionImplement
	{
		template<typename T, typename BufferType>
		inline constexpr bool CanStoreInBuffer()
		{
			return (sizeof(T) <= sizeof(BufferType)) && (alignof(BufferType) % alignof(T) == 0);
		}

		template<typename BufferType>
		inline constexpr bool CanStoreInBuffer(SizeType size, SizeType alignment)
		{
			return (size <= sizeof(BufferType)) && (alignof(BufferType) % alignment == 0);
		}

		template<typename Ret, typename... Args>
		using InvokeFunction = Ret (*)(void*&, Args...);

		template<typename T, typename Ret, typename... Args>
		inline Ret Invoke(void*& ptr, Args... args)
		{
			if constexpr (CanStoreInBuffer<T, decltype(ptr)>())
				return std::invoke(*reinterpret_cast<T*>(&ptr), static_cast<Args>(args)...);
			else
				return std::invoke(*reinterpret_cast<T*>(ptr), static_cast<Args>(args)...);
		}

		template<typename T>
			requires requires(const T& t) {
				{
					t == nullptr
				} -> std::same_as<bool>;
			}
		inline constexpr bool IsNullPointer(const T& value)
		{
			return value == nullptr;
		}

		template<typename T>
		inline constexpr bool IsNullPointer(const T& value)
		{
			return false;
		}
	}

	/*!
	@brief a callable object wrapper.
	*/
	template<typename T, typename Allocator = DefaultAllocator>
	class Function
	{
	};

	struct InvalidFunctionError
	{
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("Function is invalid");
		template<typename T, typename Allocator>
		inline static bool Judge(const Function<T, Allocator>& func)
		{
			return !func.IsValid();
		}
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

		inline Function()
			: m_pInvokeFunction(nullptr), m_pMetaData(nullptr), m_pBuffer(nullptr)
		{
		}

		inline ~Function()
		{
			if (m_pInvokeFunction)
			{
				if (IsStoreInBuffer())
					m_pMetaData->m_pDestructor(&m_pBuffer);
				else
				{
					m_pMetaData->m_pDestructor(m_pBuffer);
					Allocator::RawDelete(m_pBuffer);
				}
			}
		}

		inline Function(const Function& func)
			: m_pInvokeFunction(func.m_pInvokeFunction), m_pMetaData(func.m_pMetaData)
		{
			if (m_pInvokeFunction)
			{
				SGE_ASSERT(NullPointerError, m_pMetaData->m_pCopyConstructor);
				if (IsStoreInBuffer())
					m_pMetaData->m_pCopyConstructor(&m_pBuffer, &(func.m_pBuffer));
				else
				{
					m_pBuffer = Allocator::RawNew(m_pMetaData->m_Size, m_pMetaData->m_Alignment);
					m_pMetaData->m_pCopyConstructor(m_pBuffer, func.m_pBuffer);
				}
			}
			else
				m_pBuffer = nullptr;
		}

		inline Function(Function&& func)
			: m_pInvokeFunction(func.m_pInvokeFunction), m_pMetaData(func.m_pMetaData)
		{
			if (m_pInvokeFunction)
			{
				if (IsStoreInBuffer())
				{
					SGE_ASSERT(NullPointerError, m_pMetaData->m_pMoveConstructor);
					m_pMetaData->m_pMoveConstructor(&m_pBuffer, &(func.m_pBuffer));
					m_pMetaData->m_pDestructor(&(func.m_pBuffer));
				}
				else
					m_pBuffer = func.m_pBuffer;
				func.m_pInvokeFunction = nullptr;
			}
			else
				m_pBuffer = nullptr;
		}

		inline Function& operator=(const Function& func)
		{
			SGE_ASSERT(SelfAssignmentError, this, &func);
			if (m_pInvokeFunction)
			{
				if (func.m_pInvokeFunction)
				{
					if (*m_pMetaData == *(func.m_pMetaData))
					{
						SGE_ASSERT(NullPointerError, m_pMetaData->m_pCopyAssignment);
						m_pMetaData->m_pCopyAssignment(GetData(), func.GetData());
					}
					else
					{
						SGE_ASSERT(NullPointerError, func.m_pMetaData->m_pCopyConstructor);
						RawClear();
						m_pInvokeFunction = func.m_pInvokeFunction;
						m_pMetaData = func.m_pMetaData;
						if (IsStoreInBuffer())
							m_pMetaData->m_pCopyConstructor(&m_pBuffer, &(func.m_pBuffer));
						else
						{
							m_pBuffer = Allocator::RawNew(m_pMetaData->m_Size, m_pMetaData->m_Alignment);
							m_pMetaData->m_pCopyConstructor(m_pBuffer, func.m_pBuffer);
						}
					}
				}
				else
					Clear();
			}
			else
			{
				if (func.m_pInvokeFunction)
				{
					SGE_ASSERT(NullPointerError, func.m_pMetaData->m_pCopyConstructor);
					m_pInvokeFunction = func.m_pInvokeFunction;
					m_pMetaData = func.m_pMetaData;
					if (IsStoreInBuffer())
						m_pMetaData->m_pCopyConstructor(&m_pBuffer, &(func.m_pBuffer));
					else
					{
						m_pBuffer = Allocator::RawNew(m_pMetaData->m_Size, m_pMetaData->m_Alignment);
						m_pMetaData->m_pCopyConstructor(m_pBuffer, func.m_pBuffer);
					}
				}
				// else do nothing
			}
			return *this;
		}

		inline Function& operator=(Function&& func)
		{
			SGE_ASSERT(SelfAssignmentError, this, &func);
			if (m_pInvokeFunction)
			{
				if (func.m_pInvokeFunction)
				{
					if (*m_pMetaData == *(func.m_pMetaData))
					{
						if (IsStoreInBuffer())
						{
							SGE_ASSERT(NullPointerError, m_pMetaData->m_pMoveAssignment);
							m_pMetaData->m_pMoveAssignment(&m_pBuffer, &(func.m_pBuffer));
							m_pMetaData->m_pDestructor(&(func.m_pBuffer));
						}
						else
						{
							m_pMetaData->m_pDestructor(m_pBuffer);
							Allocator::RawDelete(m_pBuffer);
							m_pBuffer = func.m_pBuffer;
						}
					}
					else
					{
						RawClear();
						m_pInvokeFunction = func.m_pInvokeFunction;
						m_pMetaData = func.m_pMetaData;
						if (IsStoreInBuffer())
						{
							SGE_ASSERT(NullPointerError, m_pMetaData->m_pMoveConstructor);
							m_pMetaData->m_pMoveConstructor(&m_pBuffer, &(func.m_pBuffer));
							m_pMetaData->m_pDestructor(&(func.m_pBuffer));
						}
						else
							m_pBuffer = func.m_pBuffer;
					}
					func.m_pInvokeFunction = nullptr;
				}
				else
					Clear();
			}
			else
			{
				if (func.m_pInvokeFunction)
				{
					m_pInvokeFunction = func.m_pInvokeFunction;
					m_pMetaData = func.m_pMetaData;
					if (IsStoreInBuffer())
					{
						SGE_ASSERT(NullPointerError, m_pMetaData->m_pMoveConstructor);
						m_pMetaData->m_pMoveConstructor(&m_pBuffer, &(func.m_pBuffer));
						m_pMetaData->m_pDestructor(&(func.m_pBuffer));
					}
					else
						m_pBuffer = func.m_pBuffer;
					func.m_pInvokeFunction = nullptr;
				}
				// else do nothing
			}
			return *this;
		}

		template<typename OtherAllocator>
		inline Function(const Function<Ret(Args...), OtherAllocator>& func)
			: m_pInvokeFunction(func.m_pInvokeFunction), m_pMetaData(func.m_pMetaData)
		{
			if (m_pInvokeFunction)
			{
				SGE_ASSERT(NullPointerError, m_pMetaData->m_pCopyConstructor);
				if (IsStoreInBuffer())
					m_pMetaData->m_pCopyConstructor(&m_pBuffer, &(func.m_pBuffer));
				else
				{
					m_pBuffer = Allocator::RawNew(m_pMetaData->m_Size, m_pMetaData->m_Alignment);
					m_pMetaData->m_pCopyConstructor(m_pBuffer, func.m_pBuffer);
				}
			}
			else
				m_pBuffer = nullptr;
		}

		template<typename OtherAllocator>
		inline Function(Function<Ret(Args...), OtherAllocator>&& func)
			: m_pInvokeFunction(func.m_pInvokeFunction), m_pMetaData(func.m_pMetaData)
		{
			if (m_pInvokeFunction)
			{
				SGE_ASSERT(NullPointerError, m_pMetaData->m_pMoveConstructor);
				if (IsStoreInBuffer())
					m_pMetaData->m_pMoveConstructor(&m_pBuffer, &(func.m_pBuffer));
				else
				{
					m_pBuffer = Allocator::RawNew(m_pMetaData->m_Size, m_pMetaData->m_Alignment);
					m_pMetaData->m_pMoveConstructor(m_pBuffer, func.m_pBuffer);
				}
			}
			else
				m_pBuffer = nullptr;
		}

		template<typename OtherAllocator>
		inline Function& operator=(const Function<Ret(Args...), OtherAllocator>& func)
		{
			if (m_pInvokeFunction)
			{
				if (func.m_pInvokeFunction)
				{
					if (*m_pMetaData == *(func.m_pMetaData))
					{
						SGE_ASSERT(NullPointerError, m_pMetaData->m_pCopyAssignment);
						m_pMetaData->m_pCopyAssignment(GetData(), func.GetData());
					}
					else
					{
						SGE_ASSERT(NullPointerError, func.m_pMetaData->m_pCopyConstructor);
						RawClear();
						m_pInvokeFunction = func.m_pInvokeFunction;
						m_pMetaData = func.m_pMetaData;
						if (IsStoreInBuffer())
							m_pMetaData->m_pCopyConstructor(&m_pBuffer, &(func.m_pBuffer));
						else
						{
							m_pBuffer = Allocator::RawNew(m_pMetaData->m_Size, m_pMetaData->m_Alignment);
							m_pMetaData->m_pCopyConstructor(m_pBuffer, func.m_pBuffer);
						}
					}
				}
				else
					Clear();
			}
			else
			{
				if (func.m_pInvokeFunction)
				{
					SGE_ASSERT(NullPointerError, func.m_pMetaData->m_pCopyConstructor);
					m_pInvokeFunction = func.m_pInvokeFunction;
					m_pMetaData = func.m_pMetaData;
					if (IsStoreInBuffer())
						m_pMetaData->m_pCopyConstructor(&m_pBuffer, &(func.m_pBuffer));
					else
					{
						m_pBuffer = Allocator::RawNew(m_pMetaData->m_Size, m_pMetaData->m_Alignment);
						m_pMetaData->m_pCopyConstructor(m_pBuffer, func.m_pBuffer);
					}
				}
				// else do nothing
			}
			return *this;
		}

		template<typename OtherAllocator>
		inline Function& operator=(Function<Ret(Args...), OtherAllocator>&& func)
		{
			if (m_pInvokeFunction)
			{
				if (func.m_pInvokeFunction)
				{
					if (*m_pMetaData == *(func.m_pMetaData))
					{
						SGE_ASSERT(NullPointerError, m_pMetaData->m_pMoveAssignment);
						m_pMetaData->m_pMoveAssignment(GetData(), func.GetData());
					}
					else
					{
						SGE_ASSERT(NullPointerError, func.m_pMetaData->m_pMoveConstructor);
						RawClear();
						m_pInvokeFunction = func.m_pInvokeFunction;
						m_pMetaData = func.m_pMetaData;
						if (IsStoreInBuffer())
							m_pMetaData->m_pMoveConstructor(&m_pBuffer, &(func.m_pBuffer));
						else
						{
							m_pBuffer = Allocator::RawNew(m_pMetaData->m_Size, m_pMetaData->m_Alignment);
							m_pMetaData->m_pMoveConstructor(m_pBuffer, func.m_pBuffer);
						}
					}
				}
				else
					Clear();
			}
			else
			{
				if (func.m_pInvokeFunction)
				{
					SGE_ASSERT(NullPointerError, func.m_pMetaData->m_pMoveConstructor);
					m_pInvokeFunction = func.m_pInvokeFunction;
					m_pMetaData = func.m_pMetaData;
					if (IsStoreInBuffer())
						m_pMetaData->m_pMoveConstructor(&m_pBuffer, &(func.m_pBuffer));
					else
					{
						m_pBuffer = Allocator::RawNew(m_pMetaData->m_Size, m_pMetaData->m_Alignment);
						m_pMetaData->m_pMoveConstructor(m_pBuffer, func.m_pBuffer);
					}
				}
				// else do nothing
			}
			return *this;
		}

		template<typename T, typename = std::enable_if_t<IsFunction<std::remove_cvref_t<T>>::Value == false, bool>>
		inline Function(T&& func)
		{
			static_assert(IsCorrectFunction<std::remove_cvref_t<T>, Ret(Args...)>::Value, "Function can only be constructed by callable object");
			if (FunctionImplement::IsNullPointer(func))
			{
				m_pInvokeFunction = nullptr;
				m_pMetaData = nullptr;
				m_pBuffer = nullptr;
			}
			else
			{
				m_pInvokeFunction = &FunctionImplement::Invoke<std::remove_cvref_t<T>, Ret, Args...>;
				m_pMetaData = &SpaceGameEngine::GetMetaData<std::remove_cvref_t<T>>();
				if constexpr (FunctionImplement::CanStoreInBuffer<std::remove_cvref_t<T>, decltype(m_pBuffer)>())
					new (&m_pBuffer) std::remove_cvref_t<T>(std::forward<T>(func));
				else
					m_pBuffer = Allocator::template New<std::remove_cvref_t<T>>(std::forward<T>(func));
			}
		}

		template<typename T, typename = std::enable_if_t<IsFunction<std::remove_cvref_t<T>>::Value == false, bool>>
		inline Function& operator=(T&& func)
		{
			static_assert(IsCorrectFunction<std::remove_cvref_t<T>, Ret(Args...)>::Value, "Function can only be constructed by callable object");
			if (m_pInvokeFunction)
			{
				if (FunctionImplement::IsNullPointer(func))
					Clear();
				else
				{
					if (*m_pMetaData == SpaceGameEngine::GetMetaData<std::remove_cvref_t<T>>())
						Assignment(std::forward<T>(func));
					else
					{
						RawClear();
						m_pInvokeFunction = &FunctionImplement::Invoke<std::remove_cvref_t<T>, Ret, Args...>;
						m_pMetaData = &SpaceGameEngine::GetMetaData<std::remove_cvref_t<T>>();
						if constexpr (FunctionImplement::CanStoreInBuffer<std::remove_cvref_t<T>, decltype(m_pBuffer)>())
							new (&m_pBuffer) std::remove_cvref_t<T>(std::forward<T>(func));
						else
							m_pBuffer = Allocator::template New<std::remove_cvref_t<T>>(std::forward<T>(func));
					}
				}
			}
			else
			{
				if (FunctionImplement::IsNullPointer(func))
				{
					// do nothing
				}
				else
				{
					m_pInvokeFunction = &FunctionImplement::Invoke<std::remove_cvref_t<T>, Ret, Args...>;
					m_pMetaData = &SpaceGameEngine::GetMetaData<std::remove_cvref_t<T>>();
					if constexpr (FunctionImplement::CanStoreInBuffer<std::remove_cvref_t<T>, decltype(m_pBuffer)>())
						new (&m_pBuffer) std::remove_cvref_t<T>(std::forward<T>(func));
					else
						m_pBuffer = Allocator::template New<std::remove_cvref_t<T>>(std::forward<T>(func));
				}
			}
			return *this;
		}

		inline void Clear()
		{
			if (m_pInvokeFunction)
			{
				if (IsStoreInBuffer())
					m_pMetaData->m_pDestructor(&m_pBuffer);
				else
				{
					m_pMetaData->m_pDestructor(m_pBuffer);
					Allocator::RawDelete(m_pBuffer);
				}
				m_pInvokeFunction = nullptr;
			}
		}

		inline bool IsValid() const
		{
			return m_pInvokeFunction;
		}

		template<typename T>
		inline T& Get()
		{
			SGE_ASSERT(InvalidFunctionError, *this);
			if constexpr (FunctionImplement::CanStoreInBuffer<T, decltype(m_pBuffer)>())
				return *reinterpret_cast<T*>(&m_pBuffer);
			else
				return *reinterpret_cast<T*>(m_pBuffer);
		}

		template<typename T>
		inline const T& Get() const
		{
			SGE_ASSERT(InvalidFunctionError, *this);
			if constexpr (FunctionImplement::CanStoreInBuffer<T, decltype(m_pBuffer)>())
				return *reinterpret_cast<T*>(&m_pBuffer);
			else
				return *reinterpret_cast<T*>(m_pBuffer);
		}

		inline const MetaData& GetMetaData() const
		{
			SGE_ASSERT(InvalidFunctionError, *this);
			return *m_pMetaData;
		}

		inline Ret operator()(Args... args) const
		{
			return m_pInvokeFunction(const_cast<void*&>(m_pBuffer), static_cast<Args>(args)...);
		}

	private:
		inline bool IsStoreInBuffer() const
		{
			return FunctionImplement::CanStoreInBuffer<decltype(m_pBuffer)>(m_pMetaData->m_Size, m_pMetaData->m_Alignment);
		}

		inline void* GetData()
		{
			return IsStoreInBuffer() ? &m_pBuffer : m_pBuffer;
		}

		inline const void* GetData() const
		{
			return IsStoreInBuffer() ? &m_pBuffer : m_pBuffer;
		}

		inline void RawClear()
		{
			if (m_pInvokeFunction)
			{
				if (IsStoreInBuffer())
					m_pMetaData->m_pDestructor(&m_pBuffer);
				else
				{
					m_pMetaData->m_pDestructor(m_pBuffer);
					Allocator::RawDelete(m_pBuffer);
				}
			}
		}

		template<typename T>
			requires std::assignable_from<std::remove_cvref_t<T>&, T>
		inline void Assignment(T&& value)	 // can assignment
		{
			*reinterpret_cast<std::remove_cvref_t<T>*>(GetData()) = std::forward<T>(value);
		}

		template<typename T>
		inline void Assignment(T&& value)	 // can not assignment
		{
			RawClear();
			m_pInvokeFunction = &FunctionImplement::Invoke<std::remove_cvref_t<T>, Ret, Args...>;
			m_pMetaData = &SpaceGameEngine::GetMetaData<std::remove_cvref_t<T>>();
			if constexpr (FunctionImplement::CanStoreInBuffer<std::remove_cvref_t<T>, decltype(m_pBuffer)>())
				new (&m_pBuffer) std::remove_cvref_t<T>(std::forward<T>(value));
			else
				m_pBuffer = Allocator::template New<std::remove_cvref_t<T>>(std::forward<T>(value));
		}

	private:
		FunctionImplement::InvokeFunction<Ret, Args...> m_pInvokeFunction;
		const MetaData* m_pMetaData;
		void* m_pBuffer;	// SSO, when T can be store in this variable, just use it
	};

	/*!
	@file
	@todo add bind and other functional support.
	*/

}

/*!
@}
*/