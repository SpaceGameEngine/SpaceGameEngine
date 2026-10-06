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
#include "CommonIntermediateRepresentationAPI.h"
#include "Container/HashMap.hpp"
#include "Utility/Utility.hpp"
#include "Utility/TypeId.hpp"

/*!
@ingroup CommonIntermediateRepresentation
@{
*/

namespace SpaceGameEngine::CommonIntermediateRepresentation
{
	struct InterfaceNotFoundError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("The interface is not found in given type.");
		static COMMON_INTERMEDIATE_REPRESENTATION_API bool Judge(bool found);
	};

	/*!
	@brief The CRTP base class which stores the offsets of the interfaces of the type `T`.
	@warning The interfaces are located by the offsets which are computed by `GetOffsetOfBase`, so
	neither the interfaces nor `T` itself can be inherited virtually. The offset of a virtual base
	class can only be got by reading the vbptr/vbtable in a real object and it depends on the most
	derived type, so it can not be cached as a constant here.
	*/
	template<typename T>
	class InterfaceContainer : public UncopyableAndUnmovable
	{
	public:
		template<typename U, typename... InterfaceTypes>
		friend class Interfaces;

		template<typename InterfaceType>
		inline bool HasInterface() const
		{
			return m_InterfaceTypeIds.Contains(GetTypeId<InterfaceType>());
		}

		template<typename InterfaceType>
		inline InterfaceType& GetInterface()
		{
			auto iter = m_InterfaceTypeIds.Find(GetTypeId<InterfaceType>());
			SGE_ASSERT(InterfaceNotFoundError, iter != m_InterfaceTypeIds.GetEnd());
			return *reinterpret_cast<InterfaceType*>(reinterpret_cast<UInt64>(static_cast<T*>(this)) + iter->m_Second);
		}

		template<typename InterfaceType>
		inline const InterfaceType& GetInterface() const
		{
			auto iter = m_InterfaceTypeIds.Find(GetTypeId<InterfaceType>());
			SGE_ASSERT(InterfaceNotFoundError, iter != m_InterfaceTypeIds.GetConstEnd());
			return *reinterpret_cast<const InterfaceType*>(reinterpret_cast<UInt64>(static_cast<const T*>(this)) + iter->m_Second);
		}

		template<typename InterfaceType>
		inline InterfaceType* QueryInterface()
		{
			auto iter = m_InterfaceTypeIds.Find(GetTypeId<InterfaceType>());
			if (iter != m_InterfaceTypeIds.GetEnd())
				return reinterpret_cast<InterfaceType*>(reinterpret_cast<UInt64>(static_cast<T*>(this)) + iter->m_Second);
			else
				return nullptr;
		}

		template<typename InterfaceType>
		inline const InterfaceType* QueryInterface() const
		{
			auto iter = m_InterfaceTypeIds.Find(GetTypeId<InterfaceType>());
			if (iter != m_InterfaceTypeIds.GetConstEnd())
				return reinterpret_cast<const InterfaceType*>(reinterpret_cast<UInt64>(static_cast<const T*>(this)) + iter->m_Second);
			else
				return nullptr;
		}

	private:
		template<typename U, typename InterfaceType>
		inline void AddInterface()
		{
			static_assert(std::is_base_of_v<T, U>, "U must be derived from T");
			m_InterfaceTypeIds.Insert(GetTypeId<InterfaceType>(), GetOffsetOfBase<U, InterfaceType>() - GetOffsetOfBase<U, T>());
		}

	private:
		HashMap<UInt64, Int64> m_InterfaceTypeIds;	  // type id -> offset
	};

	/*!
	@brief The helper base class which inherits the given interfaces and registers them to the
	`InterfaceContainer` of the type `T` automatically.
	@warning All the interfaces must be inherited non-virtually, because `AddInterface` uses
	`GetOffsetOfBase` which requires the offset of the base class to be a compile time constant.
	@warning The `InterfaceContainer` base class of `T` must be declared before this class in the
	base class list of `T`, otherwise the constructor of this class will be called before the
	`InterfaceContainer` is constructed.
	*/
	template<typename T, typename... InterfaceTypes>
	class Interfaces : public InterfaceTypes...
	{
	public:
		inline Interfaces()
		{
			(static_cast<T*>(this)->template AddInterface<T, InterfaceTypes>(), ...);
		}
	};
}

/*!
@}
*/