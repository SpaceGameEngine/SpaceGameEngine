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
#include "TypeDefinition.hpp"
#include "Platform.hpp"

#include <type_traits>
#include <atomic>

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

	/*!
	@file
	@todo use our own atomic instead of the std's.
	*/

	enum class MemoryOrder
	{
		Relaxed = (int)std::memory_order_relaxed,
		Consume = (int)std::memory_order_consume,
		Acquire = (int)std::memory_order_acquire,
		Release = (int)std::memory_order_release,
		AcquireRelease = (int)std::memory_order_acq_rel,
		SequentiallyConsistent = (int)std::memory_order_seq_cst
	};

	inline constexpr MemoryOrder MemoryOrderRelaxed = MemoryOrder::Relaxed;
	inline constexpr MemoryOrder MemoryOrderConsume = MemoryOrder::Consume;
	inline constexpr MemoryOrder MemoryOrderAcquire = MemoryOrder::Acquire;
	inline constexpr MemoryOrder MemoryOrderRelease = MemoryOrder::Release;
	inline constexpr MemoryOrder MemoryOrderAcquireRelease = MemoryOrder::AcquireRelease;
	inline constexpr MemoryOrder MemoryOrderSequentiallyConsistent = MemoryOrder::SequentiallyConsistent;

	template<typename T>
	class AtomicBase : protected std::atomic<T>
	{
	public:
		using ValueType = T;

		inline AtomicBase() noexcept
			: std::atomic<T>()
		{
		}

		inline AtomicBase(T val)
			: std::atomic<T>(val)
		{
		}

		inline AtomicBase(const AtomicBase&) = delete;

		inline T operator=(T val) noexcept
		{
			return std::atomic<T>::operator=(val);
		}

		inline T operator=(T val) volatile noexcept
		{
			return std::atomic<T>::operator=(val);
		}

		inline AtomicBase& operator=(const AtomicBase&) = delete;

		inline AtomicBase& operator=(const AtomicBase&) volatile = delete;

		inline bool IsLockFree() const noexcept
		{
			return std::atomic<T>::is_lock_free();
		}

		inline bool IsLockFree() const volatile noexcept
		{
			return std::atomic<T>::is_lock_free();
		}

		inline void Store(T val, MemoryOrder order = MemoryOrderSequentiallyConsistent) noexcept
		{
			std::atomic<T>::store(val, (std::memory_order)order);
		}

		inline void Store(T val, MemoryOrder order = MemoryOrderSequentiallyConsistent) volatile noexcept
		{
			std::atomic<T>::store(val, (std::memory_order)order);
		}

		inline T Load(MemoryOrder order = MemoryOrderSequentiallyConsistent) const noexcept
		{
			return std::atomic<T>::load((std::memory_order)order);
		}

		inline T Load(MemoryOrder order = MemoryOrderSequentiallyConsistent) const volatile noexcept
		{
			return std::atomic<T>::load((std::memory_order)order);
		}

		inline operator T() const noexcept
		{
			return std::atomic<T>::operator T();
		}

		inline operator T() const volatile noexcept
		{
			return std::atomic<T>::operator T();
		}

		inline T Exchange(T val, MemoryOrder order = MemoryOrderSequentiallyConsistent) noexcept
		{
			return std::atomic<T>::exchange(val, (std::memory_order)order);
		}

		inline T Exchange(T val, MemoryOrder order = MemoryOrderSequentiallyConsistent) volatile noexcept
		{
			return std::atomic<T>::exchange(val, (std::memory_order)order);
		}

		inline bool CompareExchangeWeak(T& expected, T val, MemoryOrder success, MemoryOrder failure) noexcept
		{
			return std::atomic<T>::compare_exchange_weak(expected, val, (std::memory_order)success, (std::memory_order)failure);
		}

		inline bool CompareExchangeWeak(T& expected, T val, MemoryOrder success, MemoryOrder failure) volatile noexcept
		{
			return std::atomic<T>::compare_exchange_weak(expected, val, (std::memory_order)success, (std::memory_order)failure);
		}

		inline bool CompareExchangeWeak(T& expected, T val, MemoryOrder order = MemoryOrderSequentiallyConsistent) noexcept
		{
			return std::atomic<T>::compare_exchange_weak(expected, val, (std::memory_order)order);
		}

		inline bool CompareExchangeWeak(T& expected, T val, MemoryOrder order = MemoryOrderSequentiallyConsistent) volatile noexcept
		{
			return std::atomic<T>::compare_exchange_weak(expected, val, (std::memory_order)order);
		}

		inline bool CompareExchangeStrong(T& expected, T val, MemoryOrder success, MemoryOrder failure) noexcept
		{
			return std::atomic<T>::compare_exchange_strong(expected, val, (std::memory_order)success, (std::memory_order)failure);
		}

		inline bool CompareExchangeStrong(T& expected, T val, MemoryOrder success, MemoryOrder failure) volatile noexcept
		{
			return std::atomic<T>::compare_exchange_strong(expected, val, (std::memory_order)success, (std::memory_order)failure);
		}

		inline bool CompareExchangeStrong(T& expected, T val, MemoryOrder order = MemoryOrderSequentiallyConsistent) noexcept
		{
			return std::atomic<T>::compare_exchange_strong(expected, val, (std::memory_order)order);
		}

		inline bool CompareExchangeStrong(T& expected, T val, MemoryOrder order = MemoryOrderSequentiallyConsistent) volatile noexcept
		{
			return std::atomic<T>::compare_exchange_strong(expected, val, (std::memory_order)order);
		}

		inline static constexpr const bool IsAlwaysLockFree = std::atomic<T>::is_always_lock_free;
	};

	template<typename T, typename C>
	class AtomicImplement : public AtomicBase<T>
	{
	};

	struct AtomicForPointerTag
	{
	};

	struct AtomicForIntegralTag
	{
	};

	struct AtomicForFloatingPointTag
	{
	};

	template<typename T>
	using Atomic = AtomicImplement<T, std::conditional_t<std::is_pointer_v<T>, AtomicForPointerTag,
														 std::conditional_t<std::is_integral_v<T>, AtomicForIntegralTag,
																			std::conditional_t<std::is_floating_point_v<T>, AtomicForFloatingPointTag, void>>>>;

	template<typename T>
	class AtomicImplement<T*, AtomicForPointerTag> : public AtomicBase<T*>
	{
	public:
		inline T* FetchAdd(AddressType offset, MemoryOrder order = MemoryOrderSequentiallyConsistent) noexcept
		{
			return std::atomic<T*>::fetch_add((std::ptrdiff_t)offset, (std::memory_order)order);
		}

		inline T* FetchAdd(AddressType offset, MemoryOrder order = MemoryOrderSequentiallyConsistent) volatile noexcept
		{
			return std::atomic<T*>::fetch_add((std::ptrdiff_t)offset, (std::memory_order)order);
		}

		inline T* FetchSub(AddressType offset, MemoryOrder order = MemoryOrderSequentiallyConsistent) noexcept
		{
			return std::atomic<T*>::fetch_sub((std::ptrdiff_t)offset, (std::memory_order)order);
		}

		inline T* FetchSub(AddressType offset, MemoryOrder order = MemoryOrderSequentiallyConsistent) volatile noexcept
		{
			return std::atomic<T*>::fetch_sub((std::ptrdiff_t)offset, (std::memory_order)order);
		}

		inline T* operator++() noexcept
		{
			return std::atomic<T*>::operator++();
		}

		inline T* operator++() volatile noexcept
		{
			return std::atomic<T*>::operator++();
		}

		inline T* operator++(int) noexcept
		{
			return std::atomic<T*>::operator++(0);
		}

		inline T* operator++(int) volatile noexcept
		{
			return std::atomic<T*>::operator++(0);
		}

		inline T* operator--() noexcept
		{
			return std::atomic<T*>::operator--();
		}

		inline T* operator--() volatile noexcept
		{
			return std::atomic<T*>::operator--();
		}

		inline T* operator--(int) noexcept
		{
			return std::atomic<T*>::operator--(0);
		}

		inline T* operator--(int) volatile noexcept
		{
			return std::atomic<T*>::operator--(0);
		}

		inline T* operator+=(AddressType offset) noexcept
		{
			return std::atomic<T*>::operator+=(offset);
		}

		inline T* operator+=(AddressType offset) volatile noexcept
		{
			return std::atomic<T*>::operator+=(offset);
		}

		inline T* operator-=(AddressType offset) noexcept
		{
			return std::atomic<T*>::operator-=(offset);
		}

		inline T* operator-=(AddressType offset) volatile noexcept
		{
			return std::atomic<T*>::operator-=(offset);
		}
	};

	template<typename T>
	class AtomicImplement<T, AtomicForFloatingPointTag> : public AtomicBase<T>
	{
	public:
#ifdef SGE_CPP20
		inline T FetchAdd(T val, MemoryOrder order = MemoryOrderSequentiallyConsistent) noexcept
		{
			return std::atomic<T>::fetch_add(val, (std::memory_order)order);
		}

		inline T FetchAdd(T val, MemoryOrder order = MemoryOrderSequentiallyConsistent) volatile noexcept
		{
			return std::atomic<T>::fetch_add(val, (std::memory_order)order);
		}

		inline T FetchSub(T val, MemoryOrder order = MemoryOrderSequentiallyConsistent) noexcept
		{
			return std::atomic<T>::fetch_sub(val, (std::memory_order)order);
		}

		inline T FetchSub(T val, MemoryOrder order = MemoryOrderSequentiallyConsistent) volatile noexcept
		{
			return std::atomic<T>::fetch_sub(val, (std::memory_order)order);
		}

		inline T operator+=(T val) noexcept
		{
			return std::atomic<T>::operator+=(val);
		}

		inline T operator+=(T val) volatile noexcept
		{
			return std::atomic<T>::operator+=(val);
		}

		inline T operator-=(T val) noexcept
		{
			return std::atomic<T>::operator-=(val);
		}

		inline T operator-=(T val) volatile noexcept
		{
			return std::atomic<T>::operator-=(val);
		}
#endif
	};

	template<typename T>
	class AtomicImplement<T, AtomicForIntegralTag> : public AtomicBase<T>
	{
	public:
		inline T FetchAdd(T val, MemoryOrder order = MemoryOrderSequentiallyConsistent) noexcept
		{
			return std::atomic<T>::fetch_add(val, (std::memory_order)order);
		}

		inline T FetchAdd(T val, MemoryOrder order = MemoryOrderSequentiallyConsistent) volatile noexcept
		{
			return std::atomic<T>::fetch_add(val, (std::memory_order)order);
		}

		inline T FetchSub(T val, MemoryOrder order = MemoryOrderSequentiallyConsistent) noexcept
		{
			return std::atomic<T>::fetch_sub(val, (std::memory_order)order);
		}

		inline T FetchSub(T val, MemoryOrder order = MemoryOrderSequentiallyConsistent) volatile noexcept
		{
			return std::atomic<T>::fetch_sub(val, (std::memory_order)order);
		}

		inline T FetchAnd(T val, MemoryOrder order = MemoryOrderSequentiallyConsistent) noexcept
		{
			return std::atomic<T>::fetch_and(val, (std::memory_order)order);
		}

		inline T FetchAnd(T val, MemoryOrder order = MemoryOrderSequentiallyConsistent) volatile noexcept
		{
			return std::atomic<T>::fetch_and(val, (std::memory_order)order);
		}

		inline T FetchOr(T val, MemoryOrder order = MemoryOrderSequentiallyConsistent) noexcept
		{
			return std::atomic<T>::fetch_or(val, (std::memory_order)order);
		}

		inline T FetchOr(T val, MemoryOrder order = MemoryOrderSequentiallyConsistent) volatile noexcept
		{
			return std::atomic<T>::fetch_or(val, (std::memory_order)order);
		}

		inline T FetchXor(T val, MemoryOrder order = MemoryOrderSequentiallyConsistent) noexcept
		{
			return std::atomic<T>::fetch_xor(val, (std::memory_order)order);
		}

		inline T FetchXor(T val, MemoryOrder order = MemoryOrderSequentiallyConsistent) volatile noexcept
		{
			return std::atomic<T>::fetch_xor(val, (std::memory_order)order);
		}

		inline T operator++() noexcept
		{
			return std::atomic<T>::operator++();
		}

		inline T operator++() volatile noexcept
		{
			return std::atomic<T>::operator++();
		}

		inline T operator++(int) noexcept
		{
			return std::atomic<T>::operator++(0);
		}

		inline T operator++(int) volatile noexcept
		{
			return std::atomic<T>::operator++(0);
		}

		inline T operator--() noexcept
		{
			return std::atomic<T>::operator--();
		}

		inline T operator--() volatile noexcept
		{
			return std::atomic<T>::operator--();
		}

		inline T operator--(int) noexcept
		{
			return std::atomic<T>::operator--(0);
		}

		inline T operator--(int) volatile noexcept
		{
			return std::atomic<T>::operator--(0);
		}

		inline T operator+=(T val) noexcept
		{
			return std::atomic<T>::operator+=(val);
		}

		inline T operator+=(T val) volatile noexcept
		{
			return std::atomic<T>::operator+=(val);
		}

		inline T operator-=(T val) noexcept
		{
			return std::atomic<T>::operator-=(val);
		}

		inline T operator-=(T val) volatile noexcept
		{
			return std::atomic<T>::operator-=(val);
		}

		inline T operator&=(T val) noexcept
		{
			return std::atomic<T>::operator&=(val);
		}

		inline T operator&=(T val) volatile noexcept
		{
			return std::atomic<T>::operator&=(val);
		}

		inline T operator|=(T val) noexcept
		{
			return std::atomic<T>::operator|=(val);
		}

		inline T operator|=(T val) volatile noexcept
		{
			return std::atomic<T>::operator|=(val);
		}

		inline T operator^=(T val) noexcept
		{
			return std::atomic<T>::operator^=(val);
		}

		inline T operator^=(T val) volatile noexcept
		{
			return std::atomic<T>::operator^=(val);
		}
	};

	/*!
	@}
	*/
}