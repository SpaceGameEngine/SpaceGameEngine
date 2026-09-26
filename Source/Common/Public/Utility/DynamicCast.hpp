/*
Copyright 2026 creatorlxd

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
#include <concepts>
#include "Utility/TypeId.hpp"

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{
	template<typename T, typename Base>
	concept CanDynamicCast = std::derived_from<T, Base> && requires(const Base& base) {
		{
			T::IsInstance(base)
		} -> std::same_as<bool>;
	};

	template<typename T, typename Base>
		requires CanDynamicCast<T, Base>
	inline T* DynamicCast(Base& base)
	{
		if (T::IsInstance(base))
			return static_cast<T*>(&base);
		return nullptr;
	}

	template<typename T, typename Base>
		requires CanDynamicCast<T, Base>
	inline const T* DynamicCast(const Base& base)
	{
		if (T::IsInstance(base))
			return static_cast<const T*>(&base);
		return nullptr;
	}

	template<typename T, typename Base>
	class DynamicCastHelperForDerived;

	/*!
	@brief Helper class for dynamic casting, use type id to implement dynamic casting.
	*/
	template<typename T>
	class DynamicCastHelperForBase
	{
	public:
		template<typename Derived, typename Base>
		friend class DynamicCastHelperForDerived;

		inline DynamicCastHelperForBase()
			: m_TypeId(GetTypeId<T>())
		{
		}

		inline DynamicCastHelperForBase(const DynamicCastHelperForBase&)
		{
			// do nothing
		}

		inline DynamicCastHelperForBase& operator=(const DynamicCastHelperForBase&)
		{
			// do nothing
			return *this;
		}

		inline static bool IsInstance(const T& base)
		{
			return true;
		}

	private:
		UInt64 m_TypeId;
	};

	/*!
	@brief Helper class for dynamic casting, use type id to implement dynamic casting.
	*/
	template<typename T, typename Base>
	class DynamicCastHelperForDerived
	{
	public:
		inline DynamicCastHelperForDerived()
		{
			static_cast<DynamicCastHelperForBase<Base>*>(static_cast<T*>(this))->m_TypeId = GetTypeId<T>();
		}

		inline static bool IsInstance(const Base& base)
		{
			return static_cast<const DynamicCastHelperForBase<Base>&>(base).m_TypeId == GetTypeId<T>();
		}
	};
}

/*!
@}
*/